#pragma once
#include "Entity3D.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>


class Scene
{
public:

	Scene(const std::string& name = "SampleScene");
	~Scene();

	const std::string GetName() const;
	void SetName(const std::string& name);

	//Entity & Hierarchy
	Entity3D* CreateEntity(const std::string& name = "Entity");
	void DestroyEntity(Entity3D* entity);
	void ReparentEntity(Entity3D* entity, Entity3D* newParent = nullptr);

	struct RenderEntities {
		std::vector<Entity3D*> opaqueEntities;
		std::vector<Entity3D*> alphaBlendEntities;
		std::vector<Entity3D*> lights;
		std::vector<Entity3D*> directional;
		std::vector<Entity3D*> point;
		std::vector<Entity3D*> spot;
	};

	RenderEntities GetRenderEntities() const;

	void OnStart();
	void OnUpdate(float deltaTime);
	void OnEnd();

	Entity3D* FindEntityByName(const std::string& name) const;
	std::vector<Entity3D*> GetEntitiesWithComponent(std::type_index type) const;
	Entity3D* GetRootEntity() const { return rootEntity.get(); }

	std::vector<Entity3D*> GetRenderableEntities() const;

private:
	std::string name;
	std::unique_ptr<Entity3D> rootEntity; // Scene root (invisible parent)
	std::unordered_map<Entity3D::EntityID, std::unique_ptr<Entity3D>> entities;

	// Internal helpers
	void RecursiveUpdate(Entity3D* entity, float deltaTime);

};

