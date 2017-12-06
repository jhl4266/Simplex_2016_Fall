#include "MyOctree.h"
#include "MyEntityManager.h"
using namespace Simplex;

MyOctree::MyOctree(int a_nMaxLevel, int a_nIdealEntityCount, vector3 a_v3Center, float a_fSize, MyOctree* a_pParent)
{
	//Init static variables
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;

	m_fSize = a_fSize;
	float halfSize = m_fSize / 2.0f;
	m_v3Center = a_v3Center;
	m_v3Min = a_v3Center - vector3(halfSize);
	m_v3Max = a_v3Center + vector3(halfSize);

	m_pParent = a_pParent;
	m_uEntities = 0;
	hasChildren = false;

	Init();
	
	//Start the subdivisions 
	//IF there is more than 1 level in this octree
	//AND there's too many entities
	if (a_nMaxLevel > 1 && m_uEntities > m_uIdealEntityCount) {
		Subdivide();
	}

	std::cout << "DONE INIT" << std::endl;
}
MyOctree::MyOctree(MyOctree const& other)
{
	m_uMaxLevel = other.m_uMaxLevel;
	m_uIdealEntityCount = other.m_uIdealEntityCount;

	m_fSize = other.m_fSize;

	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_pParent = other.m_pParent;
	memcpy(m_pChild, other.m_pChild, sizeof(m_pChild));
	hasChildren = other.hasChildren;

	m_EntityList = other.m_EntityList;
	m_uEntities = other.m_uEntities;

	m_pMeshMngr = other.m_pMeshMngr;
}
MyOctree& Simplex::MyOctree::operator=(MyOctree const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctree temp(other);
		Swap(temp);
	}
	return *this;
}
MyOctree::~MyOctree() { if (this != NULL) Release(); };
void MyOctree::Swap(MyOctree& other)
{
	std::swap(m_uMaxLevel, other.m_uMaxLevel);
	std::swap(m_uIdealEntityCount, other.m_uIdealEntityCount);

	std::swap(m_fSize, other.m_fSize);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	std::swap(m_pParent, other.m_pParent);
	std::swap(m_pChild, other.m_pChild);
	std::swap(hasChildren, other.hasChildren);

	std::swap(m_EntityList, other.m_EntityList);
	std::swap(m_uEntities, other.m_uEntities);

	std::swap(m_pMeshMngr, other.m_pMeshMngr);
}
std::vector<int> MyOctree::GetEntityList(void) { return m_EntityList; }
int MyOctree::GetEntityCount(void) { return m_uEntities; }
void MyOctree::Subdivide(void)
{
	float halfSize = m_fSize / 2.0f;
	float quarterSize = m_fSize / 4.0f;
	vector3 newCenterMin = m_v3Center - vector3(quarterSize);
	vector3 newCenterMax = m_v3Center + vector3(quarterSize);

	m_pChild[0] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, newCenterMin, halfSize, this);
	m_pChild[1] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMin.x, newCenterMin.y, newCenterMax.z), halfSize, this);
	m_pChild[2] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMin.x, newCenterMax.y, newCenterMin.z), halfSize, this);
	m_pChild[3] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMin.x, newCenterMax.y, newCenterMax.z), halfSize, this);

	m_pChild[4] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMax.x, newCenterMin.y, newCenterMin.z), halfSize, this);
	m_pChild[5] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMax.x, newCenterMin.y, newCenterMax.z), halfSize, this);
	m_pChild[6] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, vector3(newCenterMax.x, newCenterMax.y, newCenterMin.z), halfSize, this);
	m_pChild[7] = new MyOctree(m_uMaxLevel - 1, m_uIdealEntityCount, newCenterMax, halfSize, this);

	hasChildren = true;
}
bool MyOctree::IsColliding(int a_uRBIndex)
{
	MyEntityManager* temp = MyEntityManager::GetInstance();
	MyRigidBody* entityRB = temp->GetEntity((uint)a_uRBIndex)->GetRigidBody();
	vector3 entityMin = entityRB->GetMinGlobal();
	vector3 entityMax = entityRB->GetMaxGlobal();

	//Check X axis
	if (entityMax.x < m_v3Min.x || entityMin.x > m_v3Max.x) {
		return false;
	}

	//Check Y axis
	else if (entityMax.y < m_v3Min.y || entityMin.y > m_v3Max.y) {
		return false;
	}

	//Check Z azis
	else if (entityMax.z < m_v3Min.z || entityMin.z > m_v3Max.z) {
		return false;
	}

	//Colliding with all axis!
	return true;
}
bool MyOctree::IsLeaf(void) { return !hasChildren; }
std::vector<MyOctree*> MyOctree::GetLeaves(void) { return leaves; }
void MyOctree::StoreLeaves(void) 
{
	leaves = std::vector<MyOctree*>();
	StoreLeaves(leaves);
}
void MyOctree::StoreLeaves(std::vector<MyOctree*>& list)
{ 
	if (IsLeaf()) {
		list.push_back(this);
	}
	else {
		for (int i = 0; i < 8; i++) {
			m_pChild[i]->StoreLeaves(list);
		}
	}
}
void MyOctree::Display(vector3 a_v3Color)
{
	//Draw bounding box
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color);

	if (hasChildren) {
		for (int i = 0; i < 8; i++) {
			m_pChild[i]->Display(a_v3Color);
		}
	}
}
void MyOctree::Release(void) {

	m_pMeshMngr = nullptr;

	for (int i = 0; i < 8; i++) {
		if (m_pChild[i] != nullptr) {
			SafeDelete(m_pChild[i]);
		}
		else {
			break;
		}
	}
}
void MyOctree::KillBranches(void)
{
	//If it's not a leaf, kill all the children
	if (!IsLeaf()) {
		for (int i = 0; i < 7; i++) {
			m_pChild[i]->KillBranches();
		}
	}

	//Release this branch
	Release();
}
void MyOctree::Init(void)
{
	//Init Mesh Manager
	m_pMeshMngr = MeshManager::GetInstance();

	//Make the list of entities
	//If its the root node, we check every entity
	if (m_pParent == nullptr) 
	{
		MyEntityManager* temp = MyEntityManager::GetInstance();
		for (int i = 0; i < temp->GetEntityCount(); i++) {
			if (IsColliding(i)) {
				m_EntityList.push_back(i);
			}
		}
		m_uEntities = m_EntityList.size();
	}

	//else we check the entities in the parents class
	else 
	{
		std::vector<int> parentList = m_pParent->GetEntityList();
		for (int i = 0; i < parentList.size(); i++) {
			if (IsColliding(parentList[i])) {
				m_EntityList.push_back(parentList[i]);
				m_uEntities++;
			}
		}
		m_uEntities = m_EntityList.size();
	}

	GetOctantCount();
}
int MyOctree::GetOctantCount() {
	if (hasChildren) {
		m_uTotalOctants = 1 + GetOctantCount(0);
	}
	else {
		m_uTotalOctants = 1;
	}

	return m_uTotalOctants;
}
int MyOctree::GetOctantCount(int count) {
	count += 8;
	for (int i = 0; i < 8; i++) {
		if (m_pChild[i]->HasChildren()) {
			count += m_pChild[i]->GetOctantCount(0);
		}
	}

	return count;
}
bool MyOctree::HasChildren() {
	return hasChildren;
}