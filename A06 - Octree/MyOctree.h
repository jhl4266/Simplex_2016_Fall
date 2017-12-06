#ifndef __MYOCTREECLASS_H_
#define __MYOCTREECLASS_H_

#include "Definitions.h"

namespace Simplex
{
	class MyOctree
	{
		int m_uMaxLevel = 0;//will store the maximum level an octant can go to
		int m_uIdealEntityCount = 0; //will tell how many ideal Entities this object will contain


		int m_uTotalOctants = 0;

		float m_fSize = 0.0f; //Size of the octant
		vector3 m_v3Center = vector3(0.0f); //Will store the center point of the octant
		vector3 m_v3Min = vector3(0.0f); //Will store the minimum vector of the octant
		vector3 m_v3Max = vector3(0.0f); //Will store the maximum vector of the octant

		MyOctree* m_pParent = nullptr;//Will store the parent of the current octant
		MyOctree* m_pChild[8] = { nullptr };//Will store the children of the current octant
		bool hasChildren = false;

		std::vector<int> m_EntityList; //List of Entities under this octant (Index in Entity Manager)
		int m_uEntities = 0;

		std::vector<MyOctree*> leaves; //Only used on root

		MeshManager* m_pMeshMngr = nullptr;//Mesh Manager singleton

	public:
		/*
		USAGE: Constructor, will create an octant containing all MagnaEntities Instances in the Mesh
		manager	currently contains
		ARGUMENTS:
		- uint a_nMaxLevel = 2 -> Sets the maximum level of subdivision
		- uint nIdealEntityCount = 5 -> Sets the ideal level of objects per octant
		OUTPUT: class object
		*/
		MyOctree(int a_nMaxLevel = 2, int a_nIdealEntityCount = 5, vector3 m_v3Center = ZERO_V3, float a_fSize = 68.0f, MyOctree* a_pParent = nullptr);
		/*
		USAGE: Copy Constructor
		ARGUMENTS: class object to copy
		OUTPUT: class object instance
		*/
		MyOctree(MyOctree const& other);
		/*
		USAGE: Copy Assignment Operator
		ARGUMENTS: class object to copy
		OUTPUT: ---
		*/
		MyOctree& operator=(MyOctree const& other);
		/*
		USAGE: Destructor
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		~MyOctree(void);
		/*
		USAGE: Changes object contents for other object's
		ARGUMENTS:
		- MyOctant& other -> object to swap content from
		OUTPUT: ---
		*/
		void Swap(MyOctree& other);
		std::vector<int> MyOctree::GetEntityList(void);
		int MyOctree::GetEntityCount(void);
		/*
		USAGE: allocates 8 smaller octants in the child pointers
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Subdivide(void);
		/*
		USAGE: Asks if there is a collision with the Entity specified by index from
		the Bounding Object Manager
		ARGUMENTS:
		- int a_uRBIndex -> Index of the Entity in the Entity Manager
		OUTPUT: check of the collision
		*/
		bool IsColliding(int a_uRBIndex);
		/*
		USAGE: Asks the MyOctant if it does not contain any children (its a leaf)
		ARGUMENTS: ---
		OUTPUT: It contains no children
		*/
		bool IsLeaf(void);
		std::vector<MyOctree*> GetLeaves(void);
		void StoreLeaves(void);
		void StoreLeaves(std::vector<MyOctree*>& list);
		/*
		USAGE: Displays the MyOctant volume in the color specified
		ARGUMENTS:
		- vector3 a_v3Color = REYELLOW -> Color of the volume to display.
		OUTPUT: ---
		*/
		void Display(vector3 a_v3Color = C_YELLOW);
		/*
		USAGE: Deletes all children and the children of their children (almost sounds apocalyptic)
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void KillBranches(void);
		bool HasChildren(void);
		int GetOctantCount();

	private:
		/*
		USAGE: Deallocates member fields
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Release(void);
		/*
		USAGE: Allocates member fields
		ARGUMENTS: ---
		OUTPUT: ---
		*/
		void Init(void);
		int GetOctantCount(int count);
	};

}
#endif //__MYOCTREECLASS_H_