#include "Transform.h"
void Transform::Init(void)
{
	SetTranslation(glm::vec3(0));
	SetScale(glm::vec3(1));
}
void Transform::Release(void)
{
	//EMPTY
}
Transform::Transform()
{
	Init();
}
Transform::~Transform() { Release(); }
Transform::Transform(Transform& other)
{
	translation = other.translation;
	scale = other.scale;
}
Transform& Transform::operator=(Transform& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Transform temp(other);
		Swap(temp);
	}
	return *this;
}
void Transform::Swap(Transform& other)
{
	std::swap(translation, other.translation);
	std::swap(scale, other.scale);
}
void Transform::SetTranslation(vector3 translation)
{
	this->translation = translation;
	matTranslation = glm::translate(IDENTITY_M4, this->translation);
}
void Transform::SetScale(vector3 scale)
{
	this->scale = scale;
	matScale = glm::scale(IDENTITY_M4, this->scale);
}
void Transform::Translate(vector3 translation)
{
	this->translation += translation;
	matTranslation = glm::translate(IDENTITY_M4, this->translation);
}
void Transform::Scale(vector3 scale)
{
	this->scale += translation;
	matScale = glm::scale(IDENTITY_M4, this->scale);
}
matrix4 Transform::GetModelMatrix()
{
	return matScale * matTranslation;
}