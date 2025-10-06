#include "Entity3D.h"
#include "Mesh.h"
#include "TerrainComponent.h"

Entity3D::Entity3D() : id(nextID++)
{
	transform.SetOwner(this);  
}

bool Entity3D::IsRenderable() const
{
	return Entity3D::HasComponent(std::type_index(typeid(Mesh)));
	//	|| Entity3D::HasComponent(std::type_index(typeid(TerrainComponent)));
}

void Entity3D::OnStart()
{
}

void Entity3D::OnEnd()
{
}

void Entity3D::Update(float deltaTime)
{
	for (auto& pair : components) {
		pair.second->OnUpdate(deltaTime);
	}
}

void Entity3D::Draw(Shader& shader)
{
	for (auto& pair : components) {
		pair.second->OnRender(shader, transform.GetModelMatrix());
	}
}

std::atomic<Entity3D::EntityID> Entity3D::nextID{ 0 };
