/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Preoperative Stage

[Main class]
The program execution starts from the main method.

For more details, please refer to the documentation.

Developed by Yasmin Halwani		(yasmin.halwani@outlook.com)
Supervised by Dr. Osama Halabi	(ohalabi@qu.edu.qa)
Computer Science and Engineering Department
Qatar University
2014
****************************************************************************/

#include "stdafx.h"

/*=========================================================//
//========================[VARIABLES]======================//
//=========================================================*/
// ---------------- singleton class instance
CommonValues*			values;

// ---------------- basic variables for the scene
cCamera*				camera;
cLight*					light;
cHapticDeviceHandler*	handler;
cGenericHapticDevice*	hapticDevice;
cShapeSphere*			startingPoint;
cMesh*					model;

// ---------------- linked lists
Point*					createdPoints = NULL;

// ---------------- other variables
double					windowSizeW;
double					windowSizeH;
double					displayW;
double					displayH;
double					workspaceScaleFactor;
double					simulationRunning;
double					simulationFinished;
double					cameraAngleH;
double					cameraAngleV;
double					cameraDistance;  
double					modelTransparency;
bool					flagCameraInMotion;
int						mouseX;
int						mouseY;
int						mouseButton;
cVector3d				toolLocalPos;
cVector3d				prevToolLocalPos;
cVector3d				cameraPosition;

/*=========================================================//
//=======================[PROTOTYPES]======================//
//=========================================================*/

// ---------------- program startup methods
// calls the basic methods to set up the virtual environment
void					setupEnvironment(void);
// prints instructions to the user
void					printInstructions(void);
// initializes basic variables and objects in the virtual environment
void					initializeScene(void);
// initializes the properties of the haptic tool
void					initializeHapticTool(void);
// loads the main 3D model 
void					loadModel(void);
// sets up the main GLUT settings
void					setupGlutSettings(int, char**);
// resizes the window
void					resizeWindow(int, int);
// manages shortcuts in the GLUT window
void					keySelect(unsigned char, int, int);
// manages events on mouse clicking
void					mouseClick(int button, int state, int x, int y);
// manages events on mouse movement
void					mouseMove(int x, int y);
// stops the simulation and the haptic tool
void					close(void);

// ---------------- update methods
// the main graphics loop
void					updateGraphics(void);
// the main haptics loop
void					updateHaptics(void);
// updates the position of the camera
void					updateCameraPosition(void);
// starts the virtual environment's simulation after its set up
void					startSimulation(void);

// ---------------- algorithm methods
// adds a point to the points linked list
void					addPoint(cVector3d location);
// deletes the last point added to the points linked list
void					deleteLastAddedPoint();
// prints the list of points in the points linked list
void					printPoints(Point* pointsHead);
// writes the points in the points linked list to an external txt file
void					writePointsToFile(void);

/*=========================================================//
//========================[DECLARATIONS]===================//
//=========================================================*/

#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

/*=========================================================//
//=========================[MAIN]==========================//
//=========================================================*/

int main(int argc, char* argv[])
{		
	values = CommonValues::getInstance();

	setupEnvironment();
	setupGlutSettings(argc, argv);	
	loadModel();

	startSimulation();

    return (0);
}

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================PROGRAM STARTUP METHODS================//
//=========================================================*/

void setupEnvironment(void)
{
	printInstructions();
	initializeScene();
	updateCameraPosition();
	initializeHapticTool();	
}

//=========================================================//

void printInstructions(void)
{
	printf ("This is the preoperational stage. \nDraw the points in the path sequentially by pressing the haptic switch at each point in space. \nOnce done, press they key [p]\n");
		 
	printf ("Keyboard Options:\n\n");
    printf ("[x] - Exit application\n");
	printf ("[1] - Print HIP position\n");
	printf ("[-] - Reduce model transparency level\n");
	printf ("[+] - Increase model transparency level\n");
	printf ("[p] - Save points and exit\n");
    printf ("\n\n");	
}

//=========================================================//

void initializeScene(void)
{
	// initialize numeric values

	values->proxyRadius = 0.05;
	values->stiffnessMax = 0.0;
	values->forceMax = 0.0;

	windowSizeW = 512;
	windowSizeH = 512;
	displayW = 0;
	displayH = 0;

	workspaceScaleFactor = 0.0;
	simulationRunning = false;
	simulationFinished = false;

	cameraAngleH = 0;
    cameraAngleV = 45;
	cameraDistance = 3;

	// initialize virtual world objects
	
	// world
    values->world = new cWorld();
    values->world->setBackgroundColor(0.0, 0.0, 0.0);

	// camera
    camera = new cCamera(values->world);
    values->world->addChild(camera);
    camera->set( cVector3d (3.0, 0.0, 0.0),
                 cVector3d (0.0, 0.0, 0.0),
                 cVector3d (0.0, 0.0, 1.0));
    camera->setClippingPlanes(0.01, 10.0);

	// light
    light = new cLight(values->world);
    camera->addChild(light);                   
    light->setEnabled(true);                   
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  
}

//=========================================================//

void initializeHapticTool(void)
{
    handler = new cHapticDeviceHandler();
    handler->getDevice(hapticDevice, 0);

    cHapticDeviceInfo info;
    if (hapticDevice)
    {
        info = hapticDevice->getSpecifications();
    }

    values->tool = new cGeneric3dofPointer(values->world);
    camera->addChild(values->tool);
    values->tool->setPos(-cameraDistance, 0.0, 0.0);
    values->tool->setHapticDevice(hapticDevice);
    values->tool->start();
    values->tool->setWorkspaceRadius(1.0);
    values->tool->setRadius(0.02);
    values->tool->m_deviceSphere->setShowEnabled(true);

	// proxy properties
    values->tool->m_proxyPointForceModel->setProxyRadius(values->proxyRadius);
    values->tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = true;

	workspaceScaleFactor = values->tool->getWorkspaceScaleFactor();
	values->stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;
	values->forceMax = info.m_maxForce;
}

//=========================================================//

void setupGlutSettings(int argc, char** argv)
{
    glutInit(&argc, argv);

    int screenW = glutGet(GLUT_SCREEN_WIDTH);
    int screenH = glutGet(GLUT_SCREEN_HEIGHT);
    int windowPosX = (screenW - windowSizeW) / 2;
    int windowPosY = (screenH - windowSizeH) / 2;

    glutInitWindowPosition(windowPosX, windowPosY);
    glutInitWindowSize(windowSizeW, windowSizeH);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutCreateWindow(argv[0]);
    glutDisplayFunc(updateGraphics);
    glutKeyboardFunc(keySelect);
    glutReshapeFunc(resizeWindow);
	glutMouseFunc(mouseClick);
    glutMotionFunc(mouseMove);
    glutSetWindowTitle("CHAI 3D");
}

//=========================================================//

void resizeWindow(int w, int h)
{
	displayW = w;
	displayH = h;
    glViewport(0, 0, w, h);
}

//=========================================================//

void keySelect(unsigned char key, int x, int y)
{
    // escape key
    if ((key == 27) || (key == 'x'))
    {
        // close everything
        close();

        // exit application
        exit(0);
    }

	// prints the current position of the haptic tooltip
	if(key=='1')
	{
		values->tool->getDeviceGlobalPos().print();
	}

	// redues model transparency level
	if(key=='-')
	{	
		modelTransparency-=0.1;
		model->setTransparencyLevel(modelTransparency);
	}
	
	// increases model transparency level
	if(key=='+')
	{
		modelTransparency+=0.1;
		model->setTransparencyLevel(modelTransparency);
	}

	// prints out the points in the points linked list to an external txt file and exits application
	if(key=='p')
	{
		writePointsToFile();
		close();
		exit(0);
	}

}

//=========================================================//

void mouseClick(int button, int state, int x, int y)
{
	// mouse click down
     if (state == GLUT_DOWN)
    {
        flagCameraInMotion = true;
		mouseX = x;
        mouseY = y;
        mouseButton = button;
    }

	 // mouse click up
    else if (state == GLUT_UP)
    {
        flagCameraInMotion = false;
    }
}

//=========================================================//

void mouseMove(int x, int y)
{
    if (flagCameraInMotion)
    {
		// right click changes the depth 
		if (mouseButton == GLUT_RIGHT_BUTTON)
		{
			cameraDistance = cameraDistance - 0.01 * (y - mouseY);
		}

		// right click changes the rotation
        else if (mouseButton == GLUT_LEFT_BUTTON)
        {
            cameraAngleH = cameraAngleH - (x - mouseX);
            cameraAngleV = cameraAngleV + (y - mouseY);
        }
    }

    updateCameraPosition();
	
    mouseX = x;
    mouseY = y;
}

//=========================================================//

void close(void)
{
    simulationRunning = false;

    while (!simulationFinished) { cSleepMs(100); }

	values->tool->stop();
}

//=========================================================//

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================UPDATE METHODS=========================//
//=========================================================*/

void updateGraphics(void)
{
    camera->renderView(displayW, displayH);

    glutSwapBuffers();

    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));

    if (simulationRunning)
    {
        glutPostRedisplay();
    }
}

//=========================================================//

void updateCameraPosition()
{
  // check values
    if (cameraDistance < 0.1) { cameraDistance = 0.1; }
    if (cameraAngleV > 89) { cameraAngleV = 89; }
    if (cameraAngleV < -89) { cameraAngleV = -89; }

	 // compute position of camera in space
    cVector3d pos = cAdd(
                        cameraPosition,
                        cVector3d(
                            cameraDistance * cCosDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleH) * cCosDeg(cameraAngleV),
                            cameraDistance * cSinDeg(cameraAngleV)
                        )
                    );

     // compute lookat position
    cVector3d lookat = cameraPosition;

    // define role orientation of camera
    cVector3d up(0.0, 0.0, 1.0);

    // set new position to camera
    camera->set(pos, lookat, up);

    // recompute global positions
    values->world->computeGlobalPositions(true);

	// update tool position
    if (values->tool != NULL)
    values->tool->setPos(-cameraDistance, 0.0, 0.0);
}

//=========================================================//

void startSimulation()
{
    simulationRunning = true;

	// create a separate thread for the main haptic loop
    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

    glutMainLoop();

    close();
}

//=========================================================//

void updateHaptics(void)
{
	Point* tempPoints = createdPoints;

	// time variables
	time_t  lastClick;
	time_t	newClick;
	// initialize the time of last click by the haptic device to the current time
	time(&lastClick);


    while(simulationRunning)
    {
		values->world->computeGlobalPositions(true);
		values->tool->updatePose();
		values->tool->computeInteractionForces();

		toolLocalPos  = values->tool->getDeviceLocalPos();

		// the main switch changes the camera orientation
		if(values->tool->getUserSwitch(0))
		{
			 // compute tool offset
            cVector3d offset = toolLocalPos - prevToolLocalPos;

            // apply camera motion
            cameraDistance = cameraDistance - 2 * offset.x;
            cameraAngleH = cameraAngleH - 40 * offset.y;
            cameraAngleV = cameraAngleV - 40 * offset.z;

            updateCameraPosition();   
		}

		// this switch adds a point
		if(values->tool->getUserSwitch(1))
		{
			addPoint(values->tool->getDeviceGlobalPos());
		}

		// this switch deletes the last added point
		if(values->tool->getUserSwitch(3))
		{
			time(&newClick);
			if(newClick!=lastClick) // the delete operation can only happen once per second to avoid errors
				deleteLastAddedPoint();

			time(&lastClick);
		}

		prevToolLocalPos  = toolLocalPos;
 
		values->tool->applyForces();

    }
    
    // exit haptics thread
    simulationFinished = true;
}

//=========================================================//

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================ALGORITHM METHODS======================//
//=========================================================*/

void loadModel()
{
	// initial model transparency
	modelTransparency = 0.7;

	string modelPath = "../resources/heart.3DS";

	bool fileload;
	string resourceRoot;

	model = new cMesh(values->world);

	fileload = model->loadFromFile(RESOURCE_PATH(modelPath));
	if (!fileload)
	{
		#if defined(_MSVC)
		fileload = model->loadFromFile(modelPath);
		#endif
	}
	if (!fileload)
	{
		printf("Error - 3D Model failed to load correctly.\n");
	}

	// setup initial model properties
    model->computeAllNormals(true);	
    model->computeBoundaryBox(true);
    double size = cSub(model->getBoundaryMax(), model->getBoundaryMin()).length();
    model->scale((2.0 * values->tool->getWorkspaceRadius() / size)); // scale to fit in the view
	model->setTransparencyLevel(modelTransparency);
	model->setAsGhost(true); // to avoid collisions
	values->world->addChild(model);

	// the following transformation values are experimental to the 
	// heart 3D model. Use different values for different models until
	// it fits properly in the virtual world
	model->scale(3);
	model->translate(0,0,0.5);
}

//=========================================================//

void printPoints(Point* pointsHead)
{
	Point* point = pointsHead;

	// traverse through the linked list and print the points
	while( point!=NULL )
	{
		point->point.print();
		point = point->next; 
	}

	pointsHead = point;
}

//=========================================================//

void writePointsToFile(void)
{
	// create a file to store the points
	ofstream myfile ("points.txt");
	Point* point = createdPoints;
	
	// traverse through the linked list and print the points to file
	while( point!=NULL )
	{
		if (myfile.is_open()){
			myfile << point->point.x << "," << point->point.y << "," << point->point.z;
			myfile << "\n";
	  }
	  else {
		cout << "Unable to open file";
		break;
	  }
		point = point->next; 
	}

	myfile.close();

	createdPoints = point;
}

//=========================================================//

void addPoint(cVector3d location)
{
	bool isRepeated = false;
	Point* pointSearch = createdPoints;
	while( pointSearch!=NULL )
	{
		// this condition avoids adding points within a small distance (a radius of 0.3)
		// this helps in creating VF units later in the operational stage
		// at a comfortable distance for navigation
		if(pointSearch->point.distance(location) <=0.3)
		{
			isRepeated = true;
			break;
		}
		pointSearch = pointSearch->next; 
	}

	if(!isRepeated)
	{
		Point* point;

		point = (Point*)malloc(sizeof(Point)); 

		point = new Point(location);
		point->next=createdPoints;
		createdPoints = point; 
	}
	
}

//=========================================================//

void deleteLastAddedPoint()
{
	if(createdPoints!=NULL){
		Point *temp;                              
		temp = createdPoints;
		createdPoints->pointDraw->scale(0.01); // scales the point to be very small and hardly visible graphically
		if(temp->next!=NULL)createdPoints = temp->next; // makes sure the linked list is not empty then deletes the point
		else
			createdPoints = NULL;
		free(temp); // frees up the memory taken by the deleted point
	}

	printPoints(createdPoints); // prints out the points in the linked list
	printf("\n=======\n");
}

//=========================================================//