#include "Transform.h"
#include <algorithm>

Transform::Transform() :
	m_position(0.0f),
	m_rotation(glm::identity < glm::quat>()),
	m_scale(1.0f),	
	m_modelMatrix(1.0f),
	m_dirty(false)
{ }

Transform::~Transform()
{
	for (Transform* child : children) {
		child->parent = nullptr; 
	}
}

Transform::Transform(const glm::vec3& position, const glm::quat& rotation, 
	const glm::vec3& scale ) :
	m_position(position),
	m_rotation(rotation),
	m_scale(scale),
	m_modelMatrix(1.0f),
	m_dirty(true)
{ }

void Transform::SetParent(Transform * newParent)
{
	if (newParent == this || (newParent && newParent->IsChildOf(this))) {
		return;
	}

	if (parent) {
		parent->RemoveChild(this);
	}

	parent = newParent;

	if (parent) {
		parent->AddChild(this);
	}
	SetDirty();
}

void Transform::AddChild(Transform* child)
{
	if (child && child != this &&
		std::find(children.begin(), children.end(), child) == children.end()) {
		children.push_back(child);
		child->parent = this;
		child->SetDirty();
	}
}

void Transform::RemoveChild(Transform* child)
{
	auto it = std::remove(children.begin(), children.end(), child);
	if (it != children.end()) {
		children.erase(it, children.end());
		child->parent = nullptr;
		child->SetDirty();
	}
}


bool Transform::IsChildOf(const Transform* potentialParent) const
{
	if (!potentialParent) return false;

	const Transform* current = parent;
	while (current) {
		if (current == potentialParent) return true;
		current = current->parent;
	}
	return false;
}

//TRS FUNCTIONALITIES

glm::mat4 Transform::GetWorldMatrix() const
{
	if (parent) {
		return parent->GetWorldMatrix() * GetModelMatrix();
	}
	return GetModelMatrix();
}

glm::vec3 Transform::GetWorldPosition() const
{
	if (parent) {
		return parent->GetWorldPosition() +
			(parent->GetWorldRotation() * (m_position * parent->GetWorldScale()));
	}
	return m_position;
}

glm::vec3 Transform::GetWorldScale() const
{
	if (parent) {
		return parent->GetWorldScale() * m_scale;
	}
	return m_scale;
}

glm::quat Transform::GetWorldRotation() const
{
	if (parent) {
		return parent->GetWorldRotation() * m_rotation;
	}
	return m_rotation;
}

glm::mat4 Transform::GetModelMatrix() const {
	if (m_dirty) {
		m_modelMatrix = glm::translate(glm::mat4(1.0f), m_position);
		m_modelMatrix *= glm::mat4_cast(m_rotation);
		m_modelMatrix = glm::scale(m_modelMatrix, m_scale);
		m_dirty = false;
	}
	return m_modelMatrix;
}

void Transform::SetDirty() {
	m_dirty = true;
}

void Transform::SetPosition(const glm::vec3& position) {
	m_position = position;
	SetDirty();
}

void Transform::Translate(const glm::vec3& position) {
	m_position += position;
	SetDirty();
}

const glm::vec3& Transform::GetPosition() const {
	return m_position;
}

glm::vec3 Transform::GetEulerAngles() const {
	return glm::eulerAngles(m_rotation);
}

void Transform::SetRotation(const glm::quat& rotation) {
	m_rotation = rotation;
	SetDirty();
}

void Transform::Rotate(float angle_radians, const glm::vec3& axis) {
	m_rotation = glm::rotate(m_rotation, angle_radians, axis);
	SetDirty();
}

void Transform::Rotate(const glm::quat& rotation) {
	m_rotation = rotation * m_rotation;
	SetDirty();
}

const glm::quat& Transform::GetRotation() const {
	return m_rotation;
}

void Transform::SetScale(const glm::vec3& scale) {
	m_scale = scale;
	SetDirty();
}

const glm::vec3& Transform::GetScale() const {
	return m_scale;
}

void Transform::Scale(const glm::vec3& scale) {
	m_scale *= scale;
	SetDirty();
}