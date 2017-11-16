#include "MyEntityManager.h"
using namespace Simplex;
//  MyEntityManager
MyEntityManager* MyEntityManager::m_pInstance = nullptr;
void MyEntityManager::Init(void)
{
}
void MyEntityManager::Release(void)
{
}
MyEntityManager* MyEntityManager::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new MyEntityManager();
	}
	return m_pInstance;
}
void MyEntityManager::ReleaseInstance()
{
	//Delete all the entities
	while(!GetInstance()->m_entityList.empty()) {
		delete(GetInstance()->m_entityList.pop_back());
	}

	//delete the instance
	delete(GetInstance());
}
int Simplex::MyEntityManager::GetEntityIndex(String a_sUniqueID)
{
	//Iterate through the array
	for (int i = 0; i < m_uEntityCount; i++) {
		if (m_entityList[i]->GetUniqueID == a_sUniqueID) {
			return i;
		}
	}

	return -1;
}
//Accessors
Model* Simplex::MyEntityManager::GetModel(uint a_uIndex)
{
	if (a_uIndex != -1) {
		return m_entityList[a_uIndex]->GetModel();
	}

	return nullptr;
}
Model* Simplex::MyEntityManager::GetModel(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	if (index != -1) {
		return m_entityList[index]->GetModel();
	}

	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(uint a_uIndex)
{
	if (a_uIndex != -1) {
		return m_entityList[a_uIndex]->GetRigidBody();
	}

	return nullptr;
}
RigidBody* Simplex::MyEntityManager::GetRigidBody(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	if (index != -1) {
		return m_entityList[index]->GetRigidBody();
	}

	return nullptr;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(uint a_uIndex)
{
	if (a_uIndex != -1) {
		return m_entityList[a_uIndex]->GetModelMatrix();
	}

	return IDENTITY_M4;
}
matrix4 Simplex::MyEntityManager::GetModelMatrix(String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	if (index != -1) {
		return m_entityList[index]->GetModelMatrix();
	}

	return IDENTITY_M4;
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, String a_sUniqueID)
{
	int index = GetEntityIndex(a_sUniqueID);
	if (index != -1) {
		m_entityList[index]->SetModelMatrix(a_m4ToWorld);
	}
}
void Simplex::MyEntityManager::SetModelMatrix(matrix4 a_m4ToWorld, uint a_uIndex)
{
	if (a_uIndex != -1) {
		m_entityList[a_uIndex]->SetModelMatrix(a_m4ToWorld);
	}
}
//The big 3
MyEntityManager::MyEntityManager(){Init();}
MyEntityManager::MyEntityManager(MyEntityManager const& other){ }
MyEntityManager& MyEntityManager::operator=(MyEntityManager const& other) { return *this; }
MyEntityManager::~MyEntityManager(){Release();};
// other methods
void Simplex::MyEntityManager::Update(void)
{

}
void Simplex::MyEntityManager::AddEntity(String a_sFileName, String a_sUniqueID)
{

}
void Simplex::MyEntityManager::RemoveEntity(uint a_uIndex)
{

}
void Simplex::MyEntityManager::RemoveEntity(String a_sUniqueID)
{

}
String Simplex::MyEntityManager::GetUniqueID(uint a_uIndex)
{
	return "";
}
MyEntity* Simplex::MyEntityManager::GetEntity(uint a_uIndex)
{
	return nullptr;
}
void Simplex::MyEntityManager::AddEntityToRenderList(uint a_uIndex, bool a_bRigidBody)
{

}
void Simplex::MyEntityManager::AddEntityToRenderList(String a_sUniqueID, bool a_bRigidBody)
{

}