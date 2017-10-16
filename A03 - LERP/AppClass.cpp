#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Justin Levine - jhl4266@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUp(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	/*
	**	Read the number of orbits
	*/

	//Create and open an input stream
	std::ifstream in;
	in.open(configFile);

	//Read the stream word by word until we're at orbits
	String word;
	bool onOrbits = false;;
	while (!in.eof()) {
		in >> word;
		std::cout << word << std::endl;

		//If we're on the orbits part,
		//Read the value and convert it to an int
		if (onOrbits) {
			m_uOrbits = std::atoi(word.c_str());
			break;
		}

		//Check if we reached the orbits part
		if (word == "Orbits:") {
			onOrbits = true;
		}
	}


	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
	}

	//Set up the waypoints
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		//Push back a new list of vertices
		stopsList.push_back(std::vector<vector3>());

		//Get all of the waypoints for each shape
		int numStops = i + 3;
		float radius = 1.0f + (0.5f * i);

		for (uint j = 0; j < numStops; j++) {
			//Caculate the offset
			//This is the number that'll be thrown into sin and cos
			//To get x and y coordinates
			float offset = (PI / 2.0f);
			offset -= (float)j * ((2.0f * PI) / (float)numStops);

			stopsList[i].push_back(vector3(glm::sin(offset) * radius, glm::cos(offset) * radius, 0.0f));
		}

		//Setup the starting stops
		int firstStop = 1;
		stopNumbers.push_back(firstStop);
		starts.push_back(stopsList[i][firstStop]);
		ends.push_back(stopsList[i][firstStop + 1]);
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//Figure out the delta times (and the variable for if it needs to be reset)
	static DWORD startTime = GetTickCount();
	DWORD currentTime = GetTickCount();
	float fCurrentTime = (currentTime - startTime) / 1000.f;
	static float maxTime = 2.0f;
	bool lastTime = false;

	if (fCurrentTime > maxTime) {
		lastTime = true;
	}

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));

		//calculate the current position
		vector3 v3CurrentPos;

		//Lerp
		float fPercent = MapValue(fCurrentTime, 0.0f, maxTime, 0.0f, 1.0f);
		v3CurrentPos = glm::lerp(starts[i], ends[i], fPercent);

		//Check if we reached the point
		if (lastTime) {
			//Adjust the stop number
			++stopNumbers[i];
			stopNumbers[i] %= stopsList[i].size();

			//adjust the stops
			starts[i] = stopsList[i][stopNumbers[i]];
			ends[i] = stopsList[i][(stopNumbers[i] + 1) % stopsList[i].size()];
		}

		matrix4 m4Model = glm::translate(m4Offset, v3CurrentPos);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	if (lastTime) {
		startTime = GetTickCount();
	}

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
	//release GUI
	ShutdownGUI();
}