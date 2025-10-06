#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class Entity3D;

class Transform
{
public:
	Transform();
	~Transform();
	Transform(const glm::vec3& position,
		const glm::quat& rotation = glm::identity<glm::quat>(),
		const glm::vec3& scale = glm::vec3(1.0f)
		);

	void SetParent(Transform* parent);
	void AddChild(Transform* child);
	void RemoveChild(Transform* child);
	const std::vector<Transform*>& GetChildren() const { return children; }

	Entity3D* GetOwner() const { return owner; }
	void SetOwner(Entity3D* entity) { owner = entity; }

	bool IsChildOf(const Transform* potentialParent) const;

	glm::mat4 GetWorldMatrix() const;
	glm::vec3 GetWorldPosition() const;
	glm::vec3 GetWorldScale() const;
	glm::quat GetWorldRotation() const;

	glm::mat4 GetModelMatrix() const;
	void SetDirty();

	//Position
	void SetPosition(const glm::vec3& position);
	const glm::vec3& GetPosition() const;
	void Translate(const glm::vec3& position);

	//Rotation
	const glm::quat& GetRotation() const;
	void SetRotation(const glm::quat& rotation);
	void Rotate(float angle_radians, const glm::vec3& axis);
	void Rotate(const glm::quat& rotation);
	glm::vec3 GetEulerAngles() const;

	void SetScale(const glm::vec3& scale);
	const glm::vec3& GetScale() const;
	void Scale(const glm::vec3& scale);



private:

	std::vector<Transform*> children;
	Transform* parent = nullptr;
	Entity3D* owner = nullptr;

	glm::vec3 m_position;
	glm::quat m_rotation;
	glm::vec3 m_scale;
	mutable glm::mat4 m_modelMatrix;
	mutable bool m_dirty;

};

