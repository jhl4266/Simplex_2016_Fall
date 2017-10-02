#pragma once

#include "Definitions.h"

class Transform {

	vector3 translation;
	matrix4 matTranslation;

	vector3 scale;
	matrix4 matScale;

public:
	/*
	USAGE: Initialize the object's fields
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Init(void);
	/*
	USAGE: Releases the object from memory
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void Release(void);
	/*
	USAGE: Constructor
	ARGUMENTS: ---
	OUTPUT: class object
	*/
	Transform();
	/*
	USAGE: Destructor
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	~Transform();
	/*
	USAGE: Copy Constructor -> Transform does not allow to copy
	from other Transform objects, create an empty Transform
	and use the Instantiate method instead
	ARGUMENTS: class object (to copy)
	OUTPUT: class object
	*/
	Transform(Transform& other);
	/*
	USAGE: Copy Assignment Operator
	ARGUMENTS: class object (to copy)
	OUTPUT: class object
	*/
	Transform& operator=(Transform& other);	
	/*
	USAGE: Swaps the contents of the object with another object's content
	ARGUMENTS: class object  (to swap with)
	OUTPUT: ---
	*/
	void Swap(Transform& other);
	/*
	USAGE: Sets the position of the object
	ARGUMENTS: the position in world coordinates
	OUTPUT: ---
	*/
	void SetTranslation(vector3 translation);
	/*
	USAGE: Sets the scale of the object
	ARGUMENTS: The scalar vec3 values
	OUTPUT: ---
	*/
	void SetScale(vector3 scale);
	/*
	USAGE: Adjusts the translation to the object
	ARGUMENTS: The vector to be added to the current translation
	OUTPUT: ---
	*/
	void Translate(vector3 translation);
	/*
	USAGE: Adjusts the scale of the object
	ARGUMENTS: The scalar values to be multiplied to the current scale
	OUTPUT: ---
	*/
	void Scale(vector3 scale);
	/*
	USAGE: Gets the model Matrix (matScale * matTranslate)
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	matrix4 GetModelMatrix();
};