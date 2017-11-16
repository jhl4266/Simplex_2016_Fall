#include "MyRigidBody.h"
using namespace Simplex;
//Allocation
void MyRigidBody::Init(void)
{
	m_pMeshMngr = MeshManager::GetInstance();
	m_bVisibleBS = false;
	m_bVisibleOBB = true;
	m_bVisibleARBB = false;

	m_fRadius = 0.0f;

	m_v3ColorColliding = C_RED;
	m_v3ColorNotColliding = C_WHITE;

	m_v3Center = ZERO_V3;
	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;
	m_v3ARBBSize = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void MyRigidBody::Swap(MyRigidBody& a_pOther)
{
	std::swap(m_pMeshMngr, a_pOther.m_pMeshMngr);
	std::swap(m_bVisibleBS, a_pOther.m_bVisibleBS);
	std::swap(m_bVisibleOBB, a_pOther.m_bVisibleOBB);
	std::swap(m_bVisibleARBB, a_pOther.m_bVisibleARBB);

	std::swap(m_fRadius, a_pOther.m_fRadius);

	std::swap(m_v3ColorColliding, a_pOther.m_v3ColorColliding);
	std::swap(m_v3ColorNotColliding, a_pOther.m_v3ColorNotColliding);

	std::swap(m_v3Center, a_pOther.m_v3Center);
	std::swap(m_v3MinL, a_pOther.m_v3MinL);
	std::swap(m_v3MaxL, a_pOther.m_v3MaxL);

	std::swap(m_v3MinG, a_pOther.m_v3MinG);
	std::swap(m_v3MaxG, a_pOther.m_v3MaxG);

	std::swap(m_v3HalfWidth, a_pOther.m_v3HalfWidth);
	std::swap(m_v3ARBBSize, a_pOther.m_v3ARBBSize);

	std::swap(m_m4ToWorld, a_pOther.m_m4ToWorld);

	std::swap(m_CollidingRBSet, a_pOther.m_CollidingRBSet);
}
void MyRigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
	ClearCollidingList();
}
//Accessors
bool MyRigidBody::GetVisibleBS(void) { return m_bVisibleBS; }
void MyRigidBody::SetVisibleBS(bool a_bVisible) { m_bVisibleBS = a_bVisible; }
bool MyRigidBody::GetVisibleOBB(void) { return m_bVisibleOBB; }
void MyRigidBody::SetVisibleOBB(bool a_bVisible) { m_bVisibleOBB = a_bVisible; }
bool MyRigidBody::GetVisibleARBB(void) { return m_bVisibleARBB; }
void MyRigidBody::SetVisibleARBB(bool a_bVisible) { m_bVisibleARBB = a_bVisible; }
float MyRigidBody::GetRadius(void) { return m_fRadius; }
vector3 MyRigidBody::GetColorColliding(void) { return m_v3ColorColliding; }
vector3 MyRigidBody::GetColorNotColliding(void) { return m_v3ColorNotColliding; }
void MyRigidBody::SetColorColliding(vector3 a_v3Color) { m_v3ColorColliding = a_v3Color; }
void MyRigidBody::SetColorNotColliding(vector3 a_v3Color) { m_v3ColorNotColliding = a_v3Color; }
vector3 MyRigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 MyRigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 MyRigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 MyRigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 MyRigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 MyRigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 MyRigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 MyRigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void MyRigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix)
{
	//to save some calculations if the model matrix is the same there is nothing to do here
	if (a_m4ModelMatrix == m_m4ToWorld)
		return;

	//Assign the model matrix
	m_m4ToWorld = a_m4ModelMatrix;

	//Calculate the 8 corners of the cube
	//Back square
	v3Corner[0] = m_v3MinL;
	v3Corner[1] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MinL.z);
	v3Corner[2] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MinL.z);
	v3Corner[3] = vector3(m_v3MaxL.x, m_v3MaxL.y, m_v3MinL.z);

	//Front square
	v3Corner[4] = vector3(m_v3MinL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[5] = vector3(m_v3MaxL.x, m_v3MinL.y, m_v3MaxL.z);
	v3Corner[6] = vector3(m_v3MinL.x, m_v3MaxL.y, m_v3MaxL.z);
	v3Corner[7] = m_v3MaxL;

	//Place them in world space
	for (uint uIndex = 0; uIndex < 8; ++uIndex)
	{
		v3Corner[uIndex] = vector3(m_m4ToWorld * vector4(v3Corner[uIndex], 1.0f));
	}

	//Identify the max and min as the first corner
	m_v3MaxG = m_v3MinG = v3Corner[0];

	//get the new max and min for the global box
	for (uint i = 1; i < 8; ++i)
	{
		if (m_v3MaxG.x < v3Corner[i].x) m_v3MaxG.x = v3Corner[i].x;
		else if (m_v3MinG.x > v3Corner[i].x) m_v3MinG.x = v3Corner[i].x;

		if (m_v3MaxG.y < v3Corner[i].y) m_v3MaxG.y = v3Corner[i].y;
		else if (m_v3MinG.y > v3Corner[i].y) m_v3MinG.y = v3Corner[i].y;

		if (m_v3MaxG.z < v3Corner[i].z) m_v3MaxG.z = v3Corner[i].z;
		else if (m_v3MinG.z > v3Corner[i].z) m_v3MinG.z = v3Corner[i].z;
	}

	//we calculate the distance between min and max vectors
	m_v3ARBBSize = m_v3MaxG - m_v3MinG;
}
//The big 3
MyRigidBody::MyRigidBody(std::vector<vector3> a_pointList)
{
	Init();
	//Count the points of the incoming list
	uint uVertexCount = a_pointList.size();

	//If there are none just return, we have no information to create the BS from
	if (uVertexCount == 0)
		return;

	//Max and min as the first vector of the list
	m_v3MaxL = m_v3MinL = a_pointList[0];

	//Get the max and min out of the list
	for (uint i = 1; i < uVertexCount; ++i)
	{
		if (m_v3MaxL.x < a_pointList[i].x) m_v3MaxL.x = a_pointList[i].x;
		else if (m_v3MinL.x > a_pointList[i].x) m_v3MinL.x = a_pointList[i].x;

		if (m_v3MaxL.y < a_pointList[i].y) m_v3MaxL.y = a_pointList[i].y;
		else if (m_v3MinL.y > a_pointList[i].y) m_v3MinL.y = a_pointList[i].y;

		if (m_v3MaxL.z < a_pointList[i].z) m_v3MaxL.z = a_pointList[i].z;
		else if (m_v3MinL.z > a_pointList[i].z) m_v3MinL.z = a_pointList[i].z;
	}

	//with model matrix being the identity, local and global are the same
	m_v3MinG = m_v3MinL;
	m_v3MaxG = m_v3MaxL;

	//with the max and the min we calculate the center
	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;

	//we calculate the distance between min and max vectors
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;

	//Get the distance between the center and either the min or the max
	m_fRadius = glm::distance(m_v3Center, m_v3MinL);
}
MyRigidBody::MyRigidBody(MyRigidBody const& a_pOther)
{
	m_pMeshMngr = a_pOther.m_pMeshMngr;

	m_bVisibleBS = a_pOther.m_bVisibleBS;
	m_bVisibleOBB = a_pOther.m_bVisibleOBB;
	m_bVisibleARBB = a_pOther.m_bVisibleARBB;

	m_fRadius = a_pOther.m_fRadius;

	m_v3ColorColliding = a_pOther.m_v3ColorColliding;
	m_v3ColorNotColliding = a_pOther.m_v3ColorNotColliding;

	m_v3Center = a_pOther.m_v3Center;
	m_v3MinL = a_pOther.m_v3MinL;
	m_v3MaxL = a_pOther.m_v3MaxL;

	m_v3MinG = a_pOther.m_v3MinG;
	m_v3MaxG = a_pOther.m_v3MaxG;

	m_v3HalfWidth = a_pOther.m_v3HalfWidth;
	m_v3ARBBSize = a_pOther.m_v3ARBBSize;

	m_m4ToWorld = a_pOther.m_m4ToWorld;

	m_CollidingRBSet = a_pOther.m_CollidingRBSet;
}
MyRigidBody& MyRigidBody::operator=(MyRigidBody const& a_pOther)
{
	if (this != &a_pOther)
	{
		Release();
		Init();
		MyRigidBody temp(a_pOther);
		Swap(temp);
	}
	return *this;
}
MyRigidBody::~MyRigidBody() { Release(); };
//--- a_pOther Methods
void MyRigidBody::AddCollisionWith(MyRigidBody* a_pOther)
{
	/*
		check if the object is already in the colliding set, if
		the object is already there return with no changes
	*/
	auto element = m_CollidingRBSet.find(a_pOther);
	if (element != m_CollidingRBSet.end())
		return;
	// we couldn't find the object so add it
	m_CollidingRBSet.insert(a_pOther);
}
void MyRigidBody::RemoveCollisionWith(MyRigidBody* a_pOther)
{
	m_CollidingRBSet.erase(a_pOther);
}
void MyRigidBody::ClearCollidingList(void)
{
	m_CollidingRBSet.clear();
}
bool MyRigidBody::IsColliding(MyRigidBody* const a_pOther)
{
	//check if spheres are colliding as pre-test
	bool bColliding = (glm::distance(GetCenterGlobal(), a_pOther->GetCenterGlobal()) < m_fRadius + a_pOther->m_fRadius);

	//if they are colliding check the SAT
	if (bColliding)
	{
		satResults = (eSATResults)SAT(a_pOther);
		std::cout << satResults << ".";
		if (satResults != eSATResults::SAT_NONE) {
			bColliding = false;// reset to false
		}
	}

	if (bColliding) //they are colliding
	{
		this->AddCollisionWith(a_pOther);
		a_pOther->AddCollisionWith(this);
	}
	else //they are not colliding
	{
		this->RemoveCollisionWith(a_pOther);
		a_pOther->RemoveCollisionWith(this);
	}

	return bColliding;
}
uint MyRigidBody::GetSATResults() {
	return satResults;
}
void MyRigidBody::AddToRenderList(void)
{
	if (m_bVisibleBS)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
		else
			m_pMeshMngr->AddWireSphereToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(vector3(m_fRadius)), C_BLUE_CORNFLOWER);
	}
	if (m_bVisibleOBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorColliding);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_m4ToWorld, m_v3Center) * glm::scale(m_v3HalfWidth * 2.0f), m_v3ColorNotColliding);
	}
	if (m_bVisibleARBB)
	{
		if (m_CollidingRBSet.size() > 0)
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
		else
			m_pMeshMngr->AddWireCubeToRenderList(glm::translate(GetCenterGlobal()) * glm::scale(m_v3ARBBSize), C_YELLOW);
	}
}

uint MyRigidBody::SAT(MyRigidBody* const a_pOther)
{
	/*
	Your code goes here instead of this comment;

	For this method, if there is an axis that separates the two objects
	then the return will be different than 0; 1 for any separating axis
	is ok if you are not going for the extra credit, if you could not
	find a separating axis you need to return 0, there is an enum in
	Simplex that might help you [eSATResults] feel free to use it.
	(eSATResults::SAT_NONE has a value of 0)
	*/
	
	//Local min and maxs
	vector3 minLocal = GetMinLocal();
	vector3 minLocalAsGlobal = vector3(m_m4ToWorld * vector4(minLocal, 1.0f));
	vector3 maxLocal = GetMaxLocal();
	vector3 maxLocalAsGlobal = vector3(m_m4ToWorld * vector4(maxLocal, 1.0f));

	//vectr3[8] for the corners
	vector3* points = GetCorners();

	vector3 otherMinLocal = a_pOther->GetMinLocal();
	vector3 otherMinLocalAsGlobal = vector3(m_m4ToWorld * vector4(otherMinLocal, 1.0f));
	vector3 otherMaxLocal = a_pOther->GetMaxLocal();
	vector3 otherMaxLocalAsGlobal = vector3(m_m4ToWorld * vector4(otherMaxLocal, 1.0f));

	//vectr3[8] for the corners
	vector3* otherPoints = a_pOther->GetCorners();

	/*
	*	AX check
	*/

	//Get the Ax axis
	//points[1] holds the value with max x, min y and z
	vector3 inLocalXDirection = points[1];
	vector3 Ax = glm::normalize(inLocalXDirection - minLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, Ax)) {
		return eSATResults::SAT_AX;
	}
	
	/*
	*	AY check
	*/

	//Get the Ay axis
	//points[2] holds the value with max y, min x and z
	vector3 inLocalYDirection = points[2];
	vector3 Ay = glm::normalize(inLocalYDirection - minLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, Ay)) {
		return eSATResults::SAT_AY;
	}

	/*
	*	AZ check
	*/

	//Get the Az axis
	//points[4] holds the value with max z, min x and y
	vector3 inLocalZDirection = points[4];
	vector3 Az = glm::normalize(inLocalZDirection - minLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, Az)) {
		return eSATResults::SAT_AZ;
	}

	/*
	*	Bx check
	*/

	//Get the Bx Bxis
	inLocalXDirection = otherPoints[1];
	vector3 Bx = glm::normalize(inLocalXDirection - otherMinLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, Bx)) {
		return eSATResults::SAT_BX;
	}

	/*
	*	BY check
	*/

	//Get the By axis
	inLocalYDirection = otherPoints[2];
	vector3 By = glm::normalize(inLocalYDirection - otherMinLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, By)) {
		return eSATResults::SAT_BY;
	}

	/*
	*	BZ check
	*/

	//Get the Bz axis
	inLocalZDirection = otherPoints[4];
	vector3 Bz = glm::normalize(inLocalZDirection - otherMinLocalAsGlobal);
	if (!ProjectionsOverlap(points, otherPoints, Bz)) {
		return eSATResults::SAT_BZ;
	}

	/*
	*	Ax x Bx check
	*/

	//Get the Ax x Bx axis
	vector3 AxBx = glm::normalize(glm::cross(Ax, Bx));
	if (!ProjectionsOverlap(points, otherPoints, AxBx)) {
		return eSATResults::SAT_AXxBX;
	}

	/*
	*	Ax x By check
	*/

	//Get the Ax x By axis
	vector3 AxBy = glm::normalize(glm::cross(Ax, By));
	if (!ProjectionsOverlap(points, otherPoints, AxBy)) {
		return eSATResults::SAT_AXxBY;
	}

	/*
	*	Ax x Bz check
	*/

	//Get the Ax x Bz axis
	vector3 AxBz = glm::normalize(glm::cross(Ax, Bz));
	if (!ProjectionsOverlap(points, otherPoints, AxBz)) {
		return eSATResults::SAT_AXxBZ;
	}

	/*
	*	Ay x Bx check
	*/

	//Get the Ay x Bx axis
	vector3 AyBx = glm::normalize(glm::cross(Ay, Bx));
	if (!ProjectionsOverlap(points, otherPoints, AyBx)) {
		return eSATResults::SAT_AYxBX;
	}

	/*
	*	Ay x By check
	*/

	//Get the Ay x By axis
	vector3 AyBy = glm::normalize(glm::cross(Ay, By));
	if (!ProjectionsOverlap(points, otherPoints, AyBy)) {
		return eSATResults::SAT_AYxBY;
	}

	/*
	*	Ay x Bz check
	*/

	//Get the Ay x Bz axis
	vector3 AyBz = glm::normalize(glm::cross(Ay, Bz));
	if (!ProjectionsOverlap(points, otherPoints, AyBz)) {
		return eSATResults::SAT_AYxBZ;
	}

	/*
	*	Az x Bx check
	*/

	//Get the Az x Bx axis
	vector3 AzBx = glm::normalize(glm::cross(Az, Bx));
	if (!ProjectionsOverlap(points, otherPoints, AzBx)) {
		return eSATResults::SAT_AZxBX;
	}

	/*
	*	Az x By check
	*/

	//Get the Ax x By axis
	vector3 AzBy = glm::normalize(glm::cross(Az, By));
	if (!ProjectionsOverlap(points, otherPoints, AzBy)) {
		return eSATResults::SAT_AZxBY;
	}

	/*
	*	Az x Bz check
	*/

	//Get the Az x Bz axis
	vector3 AzBz = glm::normalize(glm::cross(Az, Bz));
	if (!ProjectionsOverlap(points, otherPoints, AzBz)) {
		return eSATResults::SAT_AZxBZ;
	}

	//there is no axis test that separates this two objects
	return eSATResults::SAT_NONE;
}

bool MyRigidBody::ProjectionsOverlap(vector3 points[], vector3 otherPoints[], vector3 axis) {
	
	//set min for both sets of points on the axis
	//Assume the 1st point for both
	vector3 min = points[0];
	vector3 max = min;

	vector3 otherMin = otherPoints[0];
	vector3 otherMax = otherMin;

	//Now loop through all the other points, find the real values
	//Min means everything is in front of,
	//Max means everything is behind
	for (int i = 1; i < 8; i++) {

		//Checking a new point in the first list
		vector3 point = points[i];

		//Is it behind the min?
		vector3 differenceMin = point - min;
		float dotMin = glm::dot(differenceMin, axis);
		if (dotMin < 0) {
			min = point;
		}

		//Is it in front of the max?
		vector3 differenceMax = point - max;
		float dotMax = glm::dot(differenceMax, axis);
		if (dotMax > 0) {
			max = point;
		}

		//Checking a new point in the other list
		vector3 otherPoint = otherPoints[i];

		//Is it behind the min?
		vector3 otherDifferenceMin = otherPoint - otherMin;
		float otherDotMin = glm::dot(otherDifferenceMin, axis);
		if (otherDotMin < 0) {
			otherMin = otherPoint;
		}

		//Is it in front of the max?
		vector3 otherDifferenceMax = otherPoint - otherMax;
		float otherDotMax = glm::dot(otherDifferenceMax, axis);
		if (otherDotMax > 0) {
			otherMax = otherPoint;
		}
	}

	//Now we hav both min and maxs
	//We need either both other points to be in front of both points or 
	//bot other points to be behind both points

	//Start by checking otherMax
	vector3 differenceOtherMaxToMax = otherMax - max;
	float dotOtherMaxToMax = glm::dot(differenceOtherMaxToMax, axis);

	vector3 differenceOtherMaxToMin = otherMax - min;
	float dotOtherMaxToMin = glm::dot(differenceOtherMaxToMin, axis);

	bool otherMaxesInFront = false;
	if (dotOtherMaxToMax > 0 && dotOtherMaxToMin > 0) {
		otherMaxesInFront = true;
	}
	else if (dotOtherMaxToMax < 0 && dotOtherMaxToMin < 0) {
		otherMaxesInFront = false;
	}
	else {
		return true;
	}

	//Now lets look at the other min
	vector3 differenceOtherMinToMax = otherMin - max;
	float dotOtherMinToMax = glm::dot(differenceOtherMinToMax, axis);

	vector3 differenceOtherMinToMin = otherMin - min;
	float dotOtherMinToMin = glm::dot(differenceOtherMinToMin, axis);

	bool otherMinsInFront = false;
	if (dotOtherMinToMax > 0 && dotOtherMinToMin > 0) {
		otherMinsInFront = true;
	}
	else if (dotOtherMinToMax < 0 && dotOtherMinToMin < 0) {
		otherMinsInFront = false;
	}
	else {
		return true;
	}

	//If they're differnet, collision still happened!
	return (otherMaxesInFront != otherMinsInFront);
}

vector3* MyRigidBody::GetCorners() {
	return v3Corner;
}

