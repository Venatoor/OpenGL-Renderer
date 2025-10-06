#include "Scene.h"
#include "Light.h"
#include "Mesh.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

Scene::Scene(const std::string& name) : name(name)
{
	rootEntity = std::make_unique<Entity3D>();
	rootEntity->name = "SceneRoot";
}

Scene::~Scene()
{
	OnEnd();
	entities.clear();
}

const std::string Scene::GetName() const
{
	return std::string();
}

void Scene::SetName(const std::string& name)
{
}

Entity3D* Scene::CreateEntity(const std::string& name)
{
	auto entity = std::make_unique<Entity3D>();

	entity->name = name;
	entity->GetTransform().SetParent(&rootEntity->GetTransform());

	Entity3D* rawPtr = entity.get();
	entities[rawPtr->GetID()] = std::move(entity);
	return rawPtr;
}

// i dont like this method if an entity is destroyed all of it's children should be destroyed too
void Scene::DestroyEntity(Entity3D* entity)
{
	if (!entity || entity == rootEntity.get()) return;

	for (auto& child : entity->GetTransform().GetChildren()) {
		if (auto childEntity = dynamic_cast<Entity3D*>(child->GetOwner())) {
			DestroyEntity(childEntity);
		}
	}
	entities.erase(entity->GetID());
}

void Scene::ReparentEntity(Entity3D* entity, Entity3D* newParent)
{
	Transform* newParentTransform = newParent ? &newParent->GetTransform() : &rootEntity->GetTransform();
	entity->GetTransform().SetParent(newParentTransform);
}

Scene::RenderEntities Scene::GetRenderEntities() const
{
	RenderEntities result;
	for (auto& pair : entities) {
		Entity3D* entity = pair.second.get();

		if (entity->HasComponent(typeid(Light))) {
			result.lights.push_back(entity);
		}

		if (entity->HasComponent(typeid(DirectionalLight))) {
			result.directional.push_back(entity);
		}

		if (entity->HasComponent(typeid(SpotLight))) {
			result.spot.push_back(entity);
		}

		if (entity->HasComponent(typeid(PointLight))) {
			result.point.push_back(entity);
		}

		if (entity->IsRenderable()) {
			if (auto mesh = entity->GetComponent<Mesh>()) {
				if (auto material = mesh->GetMaterial()) {
					if (material->GetBlendMode() == BlendMode::ALPHA) {
						result.alphaBlendEntities.push_back(entity);
					}
					else {
						result.opaqueEntities.push_back(entity);
					}
				}
			}
		}
	}
	return result;
}

void Scene::OnStart()
{
	for (auto& pair : entities) {
		pair.second->OnStart();
	}
}

void Scene::OnUpdate(float deltaTime)
{
	RecursiveUpdate(rootEntity.get(), deltaTime);
}

void Scene::OnEnd()
{
	for (auto& pair : entities) {
		pair.second->OnEnd();
	}
	entities.clear();
}

Entity3D* Scene::FindEntityByName(const std::string& name) const
{
	for (auto& pair : entities) {
		if (pair.second->name == name) return pair.second.get();
	}
	return nullptr;
}

std::vector<Entity3D*> Scene::GetEntitiesWithComponent(std::type_index type) const
{
	std::vector<Entity3D*> result;
	for (auto& pair : entities) {
		if (pair.second->HasComponent(type)) {
			result.push_back(pair.second.get());
		}
	}
	return result;
}

std::vector<Entity3D*> Scene::GetRenderableEntities() const
{
	std::vector<Entity3D*> result;
	for (auto& pair : entities) {
		if (pair.second->IsRenderable()) {
			result.push_back(pair.second.get());
		}
	}
	return result;
}

void Scene::RecursiveUpdate(Entity3D* entity, float deltaTime)
{
	entity->Update(deltaTime);
	for (auto& child : entity->GetTransform().GetChildren()) {
		if (Entity3D* childEntity = dynamic_cast<Entity3D*>(child->GetOwner())) {
			RecursiveUpdate(childEntity, deltaTime);
		}
	}
}

