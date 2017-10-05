#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	
	vector3 tip = vector3(0, a_fHeight / 2.0f, 0);
	vector3 baseMiddle = vector3(0, -a_fHeight / 2.0f, 0);

	//Controls how farbase points are from each other
	//Based off of the subdivisions
	float baseOffsetStep = (2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++) {

		//The 1st variable base point
		float offsetA = baseOffsetStep * i;
		vector3 basePointA = vector3(a_fRadius * glm::sin(offsetA), -a_fHeight / 2.0f, a_fRadius * glm::cos(offsetA));

		//The 2nd variable base point
		float offsetB = baseOffsetStep * (i + 1);
		vector3 basePointB = vector3(a_fRadius * glm::sin(offsetB), -a_fHeight / 2.0f, a_fRadius * glm::cos(offsetB));

		//Add 2 triangles...
		//1 for the side,
		//1 for the base
		AddTri(basePointA, basePointB, tip);
		AddTri(basePointB, basePointA, baseMiddle);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 topMiddle = vector3(0, a_fHeight / 2.0f, 0);
	vector3 bottomMiddle = vector3(0, -a_fHeight / 2.0f, 0);

	//Controls how farbase points are from each other
	//Based off of the subdivisions
	float baseOffsetStep = (2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++) {
		//The 1st two variable base point
		//one for the top, one for the bottom
		float offsetA = baseOffsetStep * i;
		float posAX = a_fRadius * glm::sin(offsetA);
		float posAZ = a_fRadius * glm::cos(offsetA);
		vector3 basePointATop = vector3(posAX, a_fHeight / 2.0f, posAZ);
		vector3 basePointABottom = vector3(posAX, -a_fHeight / 2.0f, posAZ);

		//The 2nd variable base point
		float offsetB = baseOffsetStep * (i + 1);
		float posBX = a_fRadius * glm::sin(offsetB);
		float posBZ = a_fRadius * glm::cos(offsetB);
		vector3 basePointBTop = vector3(posBX, a_fHeight / 2.0f, posBZ);
		vector3 basePointBBottom = vector3(posBX, -a_fHeight / 2.0f, posBZ);

		//Add 2 triangles...
		//1 for the top,
		//1 for the bottom
		AddTri(basePointATop, basePointBTop, topMiddle);
		AddTri(basePointBBottom, basePointABottom, bottomMiddle);

		//draw a quad for the side
		AddQuad(basePointABottom, basePointBBottom, basePointATop, basePointBTop);

	}	
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code

	//Controls how farbase points are from each other
	//Based off of the subdivisions
	float baseOffsetStep = (2 * PI) / a_nSubdivisions;

	for (int i = 0; i < a_nSubdivisions; i++) {

		//The 1st four variable base point
		//2 for the top (inner and outer), 2 for the bottom (inner and outer)
		float offsetA = baseOffsetStep * i;

		//First set of outer points
		float posAXOuter = a_fOuterRadius * glm::sin(offsetA);
		float posAZOuter = a_fOuterRadius * glm::cos(offsetA);
		vector3 basePointATopOuter = vector3(posAXOuter, a_fHeight / 2.0f, posAZOuter);
		vector3 basePointABottomOuter = vector3(posAXOuter, -a_fHeight / 2.0f, posAZOuter);

		//First set of inner points
		float posAXInner = a_fInnerRadius * glm::sin(offsetA);
		float posAZInner = a_fInnerRadius * glm::cos(offsetA);
		vector3 basePointATopInner = vector3(posAXInner, a_fHeight / 2.0f, posAZInner);
		vector3 basePointABottomInner = vector3(posAXInner, -a_fHeight / 2.0f, posAZInner);

		//The next four variable base point
		//2 for the top (inner and outer), 2 for the bottom (inner and outer)
		float offsetB = baseOffsetStep * (i + 1);

		//Second set of outer points
		float posBXOuter = a_fOuterRadius * glm::sin(offsetB);
		float posBZOuter = a_fOuterRadius * glm::cos(offsetB);
		vector3 basePointBTopOuter = vector3(posBXOuter, a_fHeight / 2.0f, posBZOuter);
		vector3 basePointBBottomOuter = vector3(posBXOuter, -a_fHeight / 2.0f, posBZOuter);

		//Second set of inner points
		float posBXInner = a_fInnerRadius * glm::sin(offsetB);
		float posBZInner = a_fInnerRadius * glm::cos(offsetB);
		vector3 basePointBTopInner = vector3(posBXInner, a_fHeight / 2.0f, posBZInner);
		vector3 basePointBBottomInner = vector3(posBXInner, -a_fHeight / 2.0f, posBZInner);

		//draw a quad for the outside
		AddQuad(basePointABottomOuter, basePointBBottomOuter, basePointATopOuter, basePointBTopOuter);

		//draw a quad for the inside
		AddQuad(basePointBBottomInner, basePointABottomInner, basePointBTopInner, basePointATopInner);

		//draw a quad for the top
		AddQuad(basePointATopOuter, basePointBTopOuter, basePointATopInner, basePointBTopInner);

		//draw a quad for the bottom
		AddQuad(basePointBBottomOuter, basePointABottomOuter, basePointBBottomInner, basePointABottomInner);

	}		
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code

	//Controls how farbase points are from each other
	//Based off of the subdivisions
	float baseOffsetStep = (2 * PI) / a_nSubdivisionsA;

	for (int i = 0; i < a_nSubdivisionsA; i++) {
		
		//Next we'll get the inner point on the xz 0,0 plane
		float offsetA = baseOffsetStep * i;
		float posAX = a_fInnerRadius * glm::sin(offsetA);
		float posAZ = a_fInnerRadius * glm::cos(offsetA);
		vector3 prevPointA = vector3(posAX, 0.0f, posAZ);

		float offsetB = baseOffsetStep * (i + 1);
		float posBX = a_fInnerRadius * glm::sin(offsetB);
		float posBZ = a_fInnerRadius * glm::cos(offsetB);
		vector3 prevPointB = vector3(posBX, 0.0f, posBZ);

		//Next we have to make a circle of points in that orientation
		float baseCircleOffsetStep = (2 * PI) / a_nSubdivisionsB;

		for (int j = 0; j < a_nSubdivisionsB + 1; j++) {

			//Now we gotta find the next point on the circle
			float offsetCircleA = baseCircleOffsetStep * j;
			float posNextAX = (a_fInnerRadius + (glm::cos(offsetCircleA) + 1) * (a_fOuterRadius - a_fInnerRadius)) * glm::sin(offsetA);
			float posNextAZ = (a_fInnerRadius + (glm::cos(offsetCircleA) + 1) * (a_fOuterRadius - a_fInnerRadius)) * glm::cos(offsetA);
			float posNextAY = (glm::sin(offsetCircleA) * (a_fOuterRadius - a_fInnerRadius));
			vector3 nextPointA = vector3(posNextAX, posNextAY, posNextAZ);

			//Now we gotta find the next next point on the circle
			float offsetCircleB = baseCircleOffsetStep * (j + 1);
			float posNextBX = (a_fInnerRadius + (glm::cos(offsetCircleB) + 1) * (a_fOuterRadius - a_fInnerRadius)) * glm::sin(offsetB);
			float posNextBZ = (a_fInnerRadius + (glm::cos(offsetCircleB) + 1) * (a_fOuterRadius - a_fInnerRadius)) * glm::cos(offsetB);
			float posNextBY = (glm::sin(offsetCircleB) * (a_fOuterRadius - a_fInnerRadius));
			vector3 nextPointB = vector3(posNextBX, posNextBY, posNextBZ);

			AddQuad(prevPointA, prevPointB, nextPointA, nextPointB);

			prevPointA = nextPointA;
			prevPointB = nextPointB;
		}

	}		
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	//if (a_nSubdivisions > 6)
	//	a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	vector3 top = vector3(0, a_fRadius, 0);
	vector3 bottom = vector3(0, -a_fRadius, 0);

	//The horizontal distance between subdivisions
	float horizontalStep = (2 * PI) / a_nSubdivisions;

	//First we draw the triangles on the top and bottom
	for (int i = 0; i < a_nSubdivisions; i++) {

		float offsetA = horizontalStep * i;
		float offsetB = horizontalStep * (i+1);

		float topY = glm::sqrt(glm::cos((2.0f / (float)a_nSubdivisions) * 0.5f * PI)) * a_fRadius;
		float bottomY = -topY;
		float radius = glm::sqrt(glm::pow2(a_fRadius) - glm::pow2(topY));

		//Drop top triangle
		float topAX = radius * glm::sin(offsetA);
		float topAZ = radius * glm::cos(offsetA);
		vector3 pointTopA = vector3(topAX, topY, topAZ);

		float topBX = radius * glm::sin(offsetB);
		float topBZ = radius * glm::cos(offsetB);
		vector3 pointTopB = vector3(topBX, topY, topBZ);

		AddTri(top, pointTopA, pointTopB);

		//Draw bottom triangle
		float bottomAX = radius * glm::sin(offsetA);
		float bottomAZ = radius * glm::cos(offsetA);
		vector3 pointBottomA = vector3(bottomAX, bottomY, bottomAZ);

		float bottomBX = radius * glm::sin(offsetB);
		float bottomBZ = radius * glm::cos(offsetB);
		vector3 pointBottomB = vector3(bottomBX, bottomY, bottomBZ);

		AddTri(bottom, pointBottomB, pointBottomA);
	}
	
	//Now we draw all the quads in the middle sections
	for (int j = 2; j < a_nSubdivisions; j++) {

		float topY = glm::sqrt(glm::cos(((float)(j + 1) / (float)a_nSubdivisions) * 0.5f * PI)) * a_fRadius;
		float bottomY = -topY;
		float radius = glm::sqrt(glm::pow2(a_fRadius) - glm::pow2(topY));

		float prevTopY = glm::sqrt(glm::cos(((float)(j) / (float)a_nSubdivisions) * 0.5f * PI)) * a_fRadius;
		float prevBottomY = -prevTopY;
		float prevRadius = glm::sqrt(glm::pow2(a_fRadius) - glm::pow2(prevTopY));

		for (int i = 0; i < a_nSubdivisions; i++) {

			float offsetA = horizontalStep * i;
			float offsetB = horizontalStep * (i + 1);

			//Drop top quad
			float prevTopAX = prevRadius * glm::sin(offsetA);
			float prevTopAZ = prevRadius * glm::cos(offsetA);
			vector3 prevTopA = vector3(prevTopAX, prevTopY, prevTopAZ);

			float prevTopBX = prevRadius * glm::sin(offsetB);
			float prevTopBZ = prevRadius * glm::cos(offsetB);
			vector3 prevTopB = vector3(prevTopBX, prevTopY, prevTopBZ);

			float topAX = radius * glm::sin(offsetA);
			float topAZ = radius * glm::cos(offsetA);
			vector3 pointTopA = vector3(topAX, topY, topAZ);

			float topBX = radius * glm::sin(offsetB);
			float topBZ = radius * glm::cos(offsetB);
			vector3 pointTopB = vector3(topBX, topY, topBZ);

			AddQuad(pointTopA, pointTopB, prevTopA, prevTopB);

			//Draw bottom quad
			float prevBottomAX = prevRadius * glm::sin(offsetA);
			float prevBottomAZ = prevRadius * glm::cos(offsetA);
			vector3 prevBottomA = vector3(prevBottomAX, prevBottomY, prevBottomAZ);

			float prevBottomBX = prevRadius * glm::sin(offsetB);
			float prevBottomBZ = prevRadius * glm::cos(offsetB);
			vector3 prevBottomB = vector3(prevBottomBX, prevBottomY, prevBottomBZ);

			float bottomAX = radius * glm::sin(offsetA);
			float bottomAZ = radius * glm::cos(offsetA);
			vector3 pointBottomA = vector3(bottomAX, bottomY, bottomAZ);

			float bottomBX = radius * glm::sin(offsetB);
			float bottomBZ = radius * glm::cos(offsetB);
			vector3 pointBottomB = vector3(bottomBX, bottomY, bottomBZ);

			AddQuad(prevBottomA, prevBottomB, pointBottomA, pointBottomB);
		}
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}