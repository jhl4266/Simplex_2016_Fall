#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Justin Levine - jhl4266@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//init the meshes
	float cubeSize = 1.0f;
	std::vector<vector2> blocks;

	/*
	**	We're gonna build the space invader from top to bottom
	**	The top left is 0, 0
	*/

	blocks.push_back(vector2(0, 2));
	blocks.push_back(vector2(0, 8));

	blocks.push_back(vector2(-1, 3));
	blocks.push_back(vector2(-1, 7));

	blocks.push_back(vector2(-2, 2));
	blocks.push_back(vector2(-2, 3));
	blocks.push_back(vector2(-2, 4));
	blocks.push_back(vector2(-2, 5));
	blocks.push_back(vector2(-2, 6));
	blocks.push_back(vector2(-2, 7));
	blocks.push_back(vector2(-2, 8));

	blocks.push_back(vector2(-3, 1));
	blocks.push_back(vector2(-3, 2));
	blocks.push_back(vector2(-3, 4));
	blocks.push_back(vector2(-3, 5));
	blocks.push_back(vector2(-3, 6));
	blocks.push_back(vector2(-3, 8));
	blocks.push_back(vector2(-3, 9));

	blocks.push_back(vector2(-4, 0));
	blocks.push_back(vector2(-4, 1));
	blocks.push_back(vector2(-4, 2));
	blocks.push_back(vector2(-4, 3));
	blocks.push_back(vector2(-4, 4));
	blocks.push_back(vector2(-4, 5));
	blocks.push_back(vector2(-4, 6));
	blocks.push_back(vector2(-4, 7));
	blocks.push_back(vector2(-4, 8));
	blocks.push_back(vector2(-4, 9));
	blocks.push_back(vector2(-4, 10));

	blocks.push_back(vector2(-5, 0));
	blocks.push_back(vector2(-5, 2));
	blocks.push_back(vector2(-5, 3));
	blocks.push_back(vector2(-5, 4));
	blocks.push_back(vector2(-5, 5));
	blocks.push_back(vector2(-5, 6));
	blocks.push_back(vector2(-5, 7));
	blocks.push_back(vector2(-5, 8));
	blocks.push_back(vector2(-5, 10));

	blocks.push_back(vector2(-6, 0));
	blocks.push_back(vector2(-6, 2));
	blocks.push_back(vector2(-6, 8));
	blocks.push_back(vector2(-6, 10));

	blocks.push_back(vector2(-7, 3));
	blocks.push_back(vector2(-7, 4));
	blocks.push_back(vector2(-7, 6));
	blocks.push_back(vector2(-7, 7));

	//Create the actual meshes
	while (!blocks.empty()) {
		vector2 position = blocks.back();

		//make the mesh
		MyMesh* mesh = new MyMesh();
		mesh->GenerateCube(cubeSize, C_BLACK);
		mesh->GetTransform()->SetTranslation(glm::vec3(position.y, position.x, 0.0f));
		meshes.push_back(mesh);
		
		blocks.pop_back();
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	//m_pMesh->GetTransform()->Translate(vector3(1.0f, 1.0f, 0.0f) * 0.016f);
	//m_pMesh->Render(m4Projection, m4View);

	for (int i = 0; i < meshes.size(); i++) {
		meshes[i]->GetTransform()->Translate(vector3(1.0f, 0.0f, 0.0f) * 0.016f);
		meshes[i]->Render(m4Projection, m4View);
	}
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < meshes.size(); i++) {
		SafeDelete(meshes[i]);
	}
	
	//SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}