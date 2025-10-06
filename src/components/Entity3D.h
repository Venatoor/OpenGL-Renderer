#pragma once
#include "Transform.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeindex>
#include "shaderClass.h"

class Component;

class Entity3D
{
public:

	using EntityID = size_t;

	EntityID GetID() const { return id; }
	std::string name;

	Entity3D();


	virtual ~Entity3D() = default;

	template<typename T, typename... Args>
	T* AddComponent(Args&&... args) {
		static_assert(std::is_base_of_v<Component, T>, "T must be a component !");
		auto type = std::type_index(typeid(T));
		components[type] = std::make_unique<T>(std::forward<Args>(args)...);
		components[type]->owner = this;
		return static_cast<T*>(components[type].get());
	}

	template<typename T>
	T* GetComponent() const {
		static_assert(std::is_base_of_v<Component, T>, "T must be a component !");
		auto type = std::type_index(typeid(T));
		auto it = components.find(type);
		return it != components.end() ? static_cast<T*>(it->second.get()) : nullptr;
	}

	bool HasComponent(std::type_index type) const {
		return components.find(type) != components.end();
	}

	template<typename T>
	void RemoveComponent() {
		static_assert(std::is_base_of_v<Component, T>, "T must be a Component");
		components.erase(typeid(T));
	}

	bool IsRenderable() const;


	
	void OnStart();

	void OnEnd();

	void Update(float deltaTime);

	void Draw(Shader& shader);

	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }

private:

	EntityID id;
	std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	Transform transform;

	static std::atomic<EntityID> nextID;

};

