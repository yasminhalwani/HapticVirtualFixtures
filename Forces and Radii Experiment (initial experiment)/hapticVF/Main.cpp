/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Initial Experiment 

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

// ---------------- other numeric values
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
bool					flagCameraInMotion;
int						mouseX;
int						mouseY;
int						mouseButton;
double					isMouseClicking;
cVector3d				toolLocalPos;
cVector3d				prevToolLocalPos;
cVector3d				cameraPosition;

// ---------------- linked lists
Point*					createdPoints = NULL;
MagneticLine*			createdLines = NULL;
VFBlock*				createdVFBlocks = NULL;
Corner*					createdCorners = NULL;

// ---------------- constants

cVector3d				pointsArray[values->POINTS_COUNT]; 

/*=========================================================//
//=======================[PROTOTYPES]======================//
//=========================================================*/

// ---------------- program startup methods
void					setupEnvironment(void);
void					initializeValues(void);
void					printInstructions(void);
void					initializeScene(void);
void					addGround(void);
void					initializeHapticTool(void);
void					defineStandardRadius(void);
void					setupGlutSettings(int, char**);
void					resizeWindow(int, int);
void					keySelect(unsigned char, int, int);
void					mouseClick(int button, int state, int x, int y);
void					mouseMove(int x, int y);
void					close(void);
void					addStartingPointGuide(void);
void					setStartingPointStatus(bool status);

// ---------------- update methods
void					updateGraphics(void);
void					updateHaptics(void);
void					updateCameraPosition(void);
void					startSimulation(void);

// ---------------- algorithm methods - functional
// this method is to be replaced with the output of mesh skeleton extraction
void					createPoints(void); 
void					createMagneticLinesFromPoints(Point* pointsHead);
void					createVFBlocksFromMagneticLines(MagneticLine* linesHead);

// ---------------- algorithm methods - visual
void					setHideBlocks(bool status, VFBlock* blocksHead);
void					setBlocksAsGhosts(bool status, VFBlock* blocksHead);

// ---------------- algorithm methods - print
void					printPoints(Point* pointsHead);
void					printLines(MagneticLine* linesHead);

// ---------------- algorithm methods - utilities
double					getAngleBetweenLines(MagneticLine* prevline, MagneticLine* line);



bool gvfEnabled = true; // guidance virtual fixtures
bool frvfEnabled = true; // forbidden-region virtual fixtures
bool graphicalDisplayEnabled = true; //graphical feedback of gvf and frvf


/*=========================================================//
//=========================[MAIN]==========================//
//=========================================================*/

int main(int argc, char* argv[])
{		
	values = CommonValues::getInstance();

	setupEnvironment();
	setupGlutSettings(argc, argv);
 
	createPoints();
	createMagneticLinesFromPoints(createdPoints);
	createVFBlocksFromMagneticLines(createdLines);	

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
	initializeValues();
	initializeScene();
	initializeHapticTool();	
	defineStandardRadius();
}

//=========================================================//

void initializeValues(void)
{
	values->proxyRadius = 0.05;
	values->stiffnessMax = 0.0;
	values->forceMax = 0.0;

	windowSizeW = 512;
	windowSizeH = 512;
	workspaceScaleFactor = 0.0;
	simulationRunning = false;
	simulationFinished = false;
	displayW = 0;
	displayH = 0;
}

//=========================================================//

void printInstructions(void)
{
	printf ("Keyboard Options:\n\n");
    printf ("[x] - Exit application\n");
	printf ("[1] - Print HIP position\n");
	printf ("[2] - Restore camera's original orientation\n");
	printf ("[-] - Zoom out\n");
	printf ("[+] - Zoom in\n");
	printf ("[q] - Enable GVF\n");
	printf ("[w] - Disbale GVF\n");
	printf ("[e] - Enable FRVF\n");
	printf ("[r] - Disable FRVF\n");
	printf ("[t] - Hide graphical display of VF\n");
	printf ("[y] - Show graphical display of VF\n");
	printf ("[c] - Increase guidance force\n");
	printf ("[v] - Decrease guidance force\n");
	printf ("[a] - Show number of collisions\n");
	printf ("[n] - Enable starting point guiding force\n");
	printf ("[m] - Disable starting point guiding force\n");
	printf ("[l] - Show the last computed global force on the HIP tool\n");
	printf ("[j] - Hide the red GVF lines\n");
	printf ("[k] - Show the red GVF lines\n");
    printf ("\n\n");	
}

//=========================================================//

void initializeScene(void)
{
    values->world = new cWorld();
    values->world->setBackgroundColor(0.0, 0.0, 0.0);

    camera = new cCamera(values->world);
    values->world->addChild(camera);
    camera->set( cVector3d (3.0, 0.0, 0.0),
                 cVector3d (0.0, 0.0, 0.0),
                 cVector3d (0.0, 0.0, 1.0));
    camera->setClippingPlanes(0.01, 10.0);

    light = new cLight(values->world);
    camera->addChild(light);                   
    light->setEnabled(true);                   
    light->setPos(cVector3d( 2.0, 0.5, 1.0));  
    light->setDir(cVector3d(-2.0, 0.5, 1.0));  

	cameraAngleH = 0;
    cameraAngleV = 0;
	cameraDistance = 3;
    updateCameraPosition();
	isMouseClicking = false;

	//addGround();
}

//=========================================================//

void addGround(void)
{
	string modelPath_ground = "../resources/ground.3DS";

	bool fileload;
	string resourceRoot;

	cMesh* ground = new cMesh(values->world);

	fileload = ground->loadFromFile(RESOURCE_PATH(modelPath_ground));
	if (!fileload)
	{
		#if defined(_MSVC)
		fileload = ground->loadFromFile(modelPath_ground);
		#endif
	}
	if (!fileload)
	{
		printf("Error - 3D Model [ground] failed to load correctly.\n");
	}

	values->world->addChild(ground);

	ground->rotate(cVector3d(0,1,0), cDegToRad(5));
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
    values->world->addChild(values->tool);
    values->tool->setPos(0.0, 0.0, 0.0);
    values->tool->setHapticDevice(hapticDevice);
    values->tool->start();
    values->tool->setWorkspaceRadius(1.0);
    values->tool->setRadius(0.02);
    values->tool->m_deviceSphere->setShowEnabled(true);
	values->tool->m_deviceSphere->setMaterial(values->brownBlank);

    values->tool->m_proxyPointForceModel->setProxyRadius(values->proxyRadius);
    values->tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = true;

	workspaceScaleFactor = values->tool->getWorkspaceScaleFactor();
	values->stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;
	values->forceMax = info.m_maxForce;
}

//=========================================================//

void defineStandardRadius(void)
{
	// the code below in this method is only for testing purposes
	// however it is important to set up the initial values of stdBlockHeight and stdBlockRadius
	// as they will be used a lot throughout the program
	VFBlock* testBlock = new VFBlock();
	values->stdBlockRadius = testBlock->getRadius();
	values->stdBlockHeight = testBlock->getHeight();
	testBlock->removeFromWorld();
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
	if(key == '6')
	{
		cVector3d force;
		values->tool->getHapticDevice()->getForce(force);
		printf("FORCE = %1.2f\n", force.length());
	}
    // escape key
    if ((key == 27) || (key == 'x'))
    {
        // close everything
        close();

        // exit application
        exit(0);
    }

	if(key=='1')
	{
		values->tool->getDeviceGlobalPos().print();
	}

	if(key=='2')
	{
		camera->set( cVector3d (3.0, 0.0, 0.0),
                cVector3d (0.0, 0.0, 0.0),
                cVector3d (0.0, 0.0, 1.0));
	}

	if(key=='0')
	{
		camera->getPos().print();
	}

	if(key=='-')
	{	
		if(cameraDistance<8)
		cameraDistance+=0.1;
	}

	if(key=='+')
	{
		if(cameraDistance>1)
		cameraDistance-=0.1;
	}


	if(key=='q')
	{
		if(!gvfEnabled){
			printf("\nGVF is now enabled\n");
			gvfEnabled = true;
			MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->setGuidance(true);
				tempLines = tempLines->next;   
			}

			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->getTopMesh()->setAsGhost(false);
				tempblocks->getBottomMesh()->setAsGhost(false);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->getTopMesh()->setAsGhost(false);
				tempcorners->getBottomMesh()->setAsGhost(false);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nGVF is already enabled\n");
	}

	if(key=='w')
	{
		if(gvfEnabled){
			printf("\nGVF is now disabled\n");
			gvfEnabled = false;
			MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->setGuidance(false);
				tempLines = tempLines->next;   
			}

			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->getTopMesh()->setAsGhost(true);
				tempblocks->getBottomMesh()->setAsGhost(true);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->getTopMesh()->setAsGhost(true);
				tempcorners->getBottomMesh()->setAsGhost(true);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nGVF is already disabled\n");
	}

	if(key=='e')
	{
		if(!frvfEnabled)
		{
			printf("\nFRVF is now enabled\n");
			frvfEnabled = true;
			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->setStiffnessStatus(true);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->setStiffnessStatus(true);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nFRVF is already enabled\n");
	}

	if(key=='r')
	{
		if(frvfEnabled)
		{
			printf("\nFRVF is now disabled\n");
			frvfEnabled = false;
			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->setStiffnessStatus(false);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->setStiffnessStatus(false);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nFRVF is already disabled\n");
	}

	if(key=='t')
	{
		if(graphicalDisplayEnabled)
		{
			printf("\nVF graphical display is now disabled\n");
			graphicalDisplayEnabled = false;
			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->setHide(true);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->setHide(true);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nVF graphical display is already disabled\n");
	}

	if(key=='y')
	{
		if(!graphicalDisplayEnabled)
		{
			printf("\nVF graphical display is now enabled\n");
			graphicalDisplayEnabled = true;
			VFBlock* tempblocks = createdVFBlocks;
			tempblocks = createdVFBlocks;
			while( tempblocks!=NULL )
			{
				tempblocks->setHide(false);
				tempblocks = tempblocks->next;   
			}

			Corner* tempcorners = createdCorners;
			tempcorners = createdCorners;
			while( tempcorners!=NULL )
			{
				tempcorners->setHide(false);
				tempcorners = tempcorners->next;   
			}
		}
		else
			printf("\nVF graphical display is already enabled\n");
	}

	if(key=='l')
	{
		printf("force = %1.2f\n", values->forceScaleFactor);
	}

	if(key=='v')
	{
		if(values->forceScaleFactor <= 0.0)
		{
			printf("\n Guidance force has reached its minimum value\n");
		}
		else
		{
			values->forceScaleFactor = values->forceScaleFactor - 0.1;
			printf("\nGuidance Force Scale Factor = %1.2f\n", values->forceScaleFactor);
			MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->scaleForce();
				tempLines = tempLines->next;   
			}
		}
	}

	if(key=='c')
	{
		if(values->forceScaleFactor >= 1.0)
		{
			printf("\n Guidance force has reached its maximum value\n");
		}
		else
		{
			values->forceScaleFactor = values->forceScaleFactor + 0.1;
			printf("\nGuidance Force Scale Factor = %1.2f\n", values->forceScaleFactor);
			MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->scaleForce();
				tempLines = tempLines->next;   
			}
		}	
	
	}

	if(key=='a')
	{
		printf("\ncollisions: %d", values->numOfCollisions);
	}

	
	if(key=='n')
	{
		if(startingPoint->getAsGhost())
		{
			printf("\nEnabled the guiding magnetic force at the starting point\n");
			setStartingPointStatus(true);
		}else
		{
			printf("\nThe guiding magnetic force at the starting point is already enabled\n");
		}
	}

	if(key=='m')
	{
		if(!startingPoint->getAsGhost())
		{
			printf("\nDisabled the guiding magnetic force at the starting point\n");
			setStartingPointStatus(false);
		}else
		{
			printf("\nThe guiding magnetic force at the starting point is already disabled\n");
		}
	}

	if(key=='l')
	{
		printf("\nLast computed global force on the HIP:");
		values->tool->m_lastComputedGlobalForce.print();
	}

	if(key=='j')
	{
		printf("\nRed magnetic lines are now visually hidden\n");
		MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->setLineTransparency(true);
				tempLines = tempLines->next;   
			}
	}

	if(key=='k')
	{
		printf("\nRed magnetic lines are no longer visually hidden\n");
		MagneticLine* tempLines = createdLines;
			while( tempLines!=NULL )
			{
				tempLines->setLineTransparency(false);
				tempLines = tempLines->next;   
			}
	}

}

//=========================================================//

void mouseClick(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        flagCameraInMotion = true;
		mouseX = x;
        mouseY = y;
        mouseButton = button;
    }

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
        if (mouseButton == GLUT_LEFT_BUTTON)
        {
			isMouseClicking = true;
            cameraAngleH = cameraAngleH - (x - mouseX);
            cameraAngleV = cameraAngleV + (y - mouseY);
			
        }
    }

    updateCameraPosition();
	
    mouseX = x;
    mouseY = y;

	isMouseClicking = false;
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
    values->tool->setPos(0.0f, 0.0f, 0.0f);
}

//=========================================================//

void startSimulation()
{
    simulationRunning = true;

    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

    glutMainLoop();

    close();
}

//=========================================================//

void updateHaptics(void)
{
	MagneticLine* tempLines = createdLines;
	VFBlock* tempBlocks = createdVFBlocks;
	Corner* tempCorners = createdCorners;

    while(simulationRunning)
    {
		values->world->computeGlobalPositions(true);
		values->tool->updatePose();
		values->tool->computeInteractionForces();

		toolLocalPos  = values->tool->getDeviceLocalPos();


		if(isMouseClicking){
			// controls camera movement with the mouse
			cVector3d offset = toolLocalPos - prevToolLocalPos;
			cameraDistance = cameraDistance - 2 * offset.x;
			cameraAngleH = cameraAngleH - 40 * offset.y;
			cameraAngleV = cameraAngleV - 40 * offset.z;

			updateCameraPosition(); 
		}

		prevToolLocalPos  = toolLocalPos;

		tempLines = createdLines;
		while( tempLines!=NULL )
		{
			tempLines->updateHaptics();
			tempLines = tempLines->next;   
		}

		tempBlocks = createdVFBlocks;
		while( tempBlocks!=NULL )
		{
			tempBlocks->updateHaptics();
			tempBlocks = tempBlocks->next;   
		}

		tempCorners = createdCorners;
		while( tempCorners!=NULL )
		{
			tempCorners->updateHaptics();
			tempCorners = tempCorners->next;   
		}

		if(values->isInsideThePath)
		{
			if(!startingPoint->getAsGhost())
				setStartingPointStatus(false);
		}else
		{
			if(startingPoint->getAsGhost())
				setStartingPointStatus(true);
		}

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

void createPoints()
{
	//================================ POINTS ARRAY

	// test path # 1 (2 points)
	//pointsArray[1] = cVector3d(-0.2, -0.5,  0.5);
	//pointsArray[0] = cVector3d(-0.9,  0.5, -0.0);

	// test path # 2 (5 points)
	//pointsArray[4] = cVector3d( 0.4, -0.5,  0.7);
	//pointsArray[3] = cVector3d( 0.3, -0.3, -0.6);
	//pointsArray[2] = cVector3d( 0.2,  0.0, -0.9);
	//pointsArray[1] = cVector3d( 0.1,  0.3, -0.6);
	//pointsArray[0] = cVector3d( 0.0,  0.5,  0.7);

	// test path # 3 (7 points)
	//pointsArray[0] = cVector3d(-0.9, -0.7,  0.3);
	//pointsArray[1] = cVector3d(-0.3,  0.3,  0.2);
	//pointsArray[2] = cVector3d( 0.3,  0.3, -0.3);
	//pointsArray[3] = cVector3d( 0.9, -0.4, -0.4);
	//pointsArray[4] = cVector3d( 1.0, -0.7, -0.2);
	//pointsArray[5] = cVector3d( 1.2, -0.4,  0.2);
	//pointsArray[6] = cVector3d( 1.0,  0.2,  0.6);


	// experiment path (6 points)
	pointsArray[5] = cVector3d( 0.1, -0.4,  0.3);
	pointsArray[4] = cVector3d( 0.0, -0.5, -0.3);
	pointsArray[3] = cVector3d( 0.8,  0.3, -0.5);
	pointsArray[2] = cVector3d( 0.8,  0.6,  0.4);
	pointsArray[1] = cVector3d(-0.2, 0.45,  0.4);
	pointsArray[0] = cVector3d(-0.4, 0.75, 0.75);
	//=============================================

	//CAUTION: The points are being stored in reverse order!

	Point* pointsHead = NULL;
	Point* point;

	point = (Point*)malloc(sizeof(Point)); 

	for(int i=0; i<values->POINTS_COUNT; i++)
	{
		point = new Point(pointsArray[i]);
		point->next=pointsHead;
		pointsHead = point; 
	}

	createdPoints = pointsHead;

	addStartingPointGuide();
}

//=========================================================//

void addStartingPointGuide()
{
	startingPoint = new cShapeSphere(0.03);

	values->world->addChild(startingPoint);

	startingPoint->setPos(pointsArray[values->POINTS_COUNT-1]);

	values->magneticSphereMat.setStiffness(0.4 * values->stiffnessMax);
	values->magneticSphereMat.setMagnetMaxForce(0.37 * values->forceMax * values->forceScaleFactor);
	values->magneticSphereMat.setMagnetMaxDistance(values->stdBlockHeight * 3);

	startingPoint->setMaterial(values->magneticSphereMat);

	cEffectMagnet* startingPointGuidingMagneticEffect = new cEffectMagnet(startingPoint);
    startingPoint->addEffect(startingPointGuidingMagneticEffect);
}

//=========================================================//

void setStartingPointStatus(bool status)
{
	if(status)
	{
		startingPoint->setAsGhost(false);
	}
	else
	{
		startingPoint->setAsGhost(true);
	}
	
}

//=========================================================//

void printPoints(Point* pointsHead)
{
	Point* point = pointsHead;
	while( point!=NULL )
	{
		point->point.print();
		point = point->next; 
	}

	pointsHead = point;
}

//=========================================================//

void createMagneticLinesFromPoints(Point* pointsHead)
{
	Point* point = pointsHead;

	MagneticLine* linesHead = NULL;
	MagneticLine* line;
	line = (MagneticLine*)malloc(sizeof(MagneticLine)); 
	point = pointsHead;
	for(int i=0; i<values->POINTS_COUNT-1; i++)
	{
		line		= new MagneticLine(point->point, point->next->point);
		line->next	= linesHead;
		linesHead	= line; 

		if(i==1)
			line->isSecondLine = true;

		point		= point->next; 
	}

	linesHead->isLastLine = true;

	pointsHead = point;

	createdLines = linesHead;
}

//=========================================================//

void printLines(MagneticLine* linesHead)
{
	MagneticLine* line = linesHead;
	while(line!=NULL)
	{
		line->print();
		line = line->next;
	}

	linesHead = line;
}

//=========================================================//

void createVFBlocksFromMagneticLines(MagneticLine* linesHead)
{
	MagneticLine*	line		= linesHead;
	MagneticLine*	prevLine	= new MagneticLine(cVector3d(0,0,0), cVector3d(0,0,0));
	VFBlock*		prevBlock	= NULL;
	VFBlock*		blocksHead	= NULL;
	VFBlock*		block		= (VFBlock*)malloc(sizeof(VFBlock));
	Corner*			cornersHead	= NULL;
	Corner*			corner		= (Corner*)malloc(sizeof(Corner));
	int				lineCount	= 0;
	//MagneticLine*	nextLine;
	cVector3d		v1;
	cVector3d		v2;
	double			vectorLength;
	cVector3d		axis;
	double			angle;
	cQuaternion		quat;
	cMatrix3d		rotMatrix;
	cVector3d		B;
	cVector3d		P;
	cVector3d		BNew;
	double			translationDistance;
	double			theta, compTheta, radius, theta1, theta2, theta3, theta4, n, x, sf2, sf2prev, rotStep;
	double			directionAngle;
		
	while(line!=NULL)
	{
		lineCount++;

		// get the angle between the two lines
		v2 = line->getVector();
		vectorLength = v2.length();
		theta = cRadToDeg(getAngleBetweenLines(prevLine, line));

		//---------------- create and orient the cylinder along the direction vector
		block = new VFBlock();

		v1 = block->getBottomCenterGlobalPos() - block->getTopCenterGlobalPos();
		v1.normalize();
		v2 = line->getVector();
		v2.normalize();
		axis = v1.crossAndReturn(v2);
		angle = acos(v1.dot(v2));
		quat.fromAxisAngle(axis, angle);
		rotMatrix.identity();
		quat.toRotMat(rotMatrix);

		if(axis.equals(cVector3d(0,0,0))) // the case where the vector is parallel to the cylinder
		{
			directionAngle = acos((v1.dot(v2))/((v1.length())*(v2.length())));
			directionAngle = cRadToDeg(directionAngle);

			if(directionAngle !=0 )
			{
				block->rotate(cVector3d(1,0,0), 180);
			}

		}else
			block->rotate(rotMatrix);

		//------------- calculations end
		block->scaleHeight(vectorLength / values->stdBlockHeight);
		block->setPos(line->getA());
		line->setBlock(block);
	
		//translate cylinder across direction vector
		//------------- calculations start
		B = block->getPos();
		P = line->getVector();
		translationDistance = (block->getHeight()/2);
		BNew = B + (translationDistance/P.length()) * P;
		//------------- calculations end
		block->setPos(BNew);	


		//----------------------------------------remove extra height blocking the rotation units
		//------------calculations start
		radius = values->stdBlockRadius;

		compTheta	= 360 - theta;
		theta1		= compTheta - 180;							
		theta2		= (180 - theta1) / 2;	
		n			= (radius * sin(cDegToRad(theta1))) / (sin(cDegToRad(theta2)));
		theta3		= 90 - theta2;								
		theta4		= 180 - (2 * theta3);
		x			= (n * sin(cDegToRad(theta3))) / (sin(cDegToRad(theta4)));
		rotStep		= -1 * theta1/4;

		//printf("\ntheta: %1.2f", theta);
		//printf("\nradius: %1.2f", radius);
		//printf("\ncompTheta: %1.2f", compTheta);
		//printf("\ntheta1: %1.2f", theta1);
		//printf("\ntheta2: %1.2f", theta2);
		//printf("\nn: %1.2f", n);
		//printf("\ntheta3: %1.2f", theta3);
		//printf("\ntheta4: %1.2f", theta4);
		//printf("\nx: %1.2f", x);
		//printf("\n");
		
		sf2 = 1 - (x/block->getHeight());
		if(lineCount>1)sf2prev = 1 - (x/prevBlock->getHeight());

		if(lineCount>=2){

			prevBlock->scaleHeight(sf2prev);
			B = prevBlock->getPos();
			P = prevLine->getVector();
			translationDistance = x/2;
			BNew = B + (translationDistance/P.length()) * P;
			prevBlock->setPos(BNew);	
			
			block->scaleHeight(sf2);
			B = block->getPos();
			P = line->getVector();
			translationDistance = - x/2;
			BNew = B + (translationDistance/P.length()) * P;
			block->setPos(BNew);	


			//----------------------------------------create and orient the corner
			cVector3d tran;
			cVector3d cornerPos;
			cVector3d newCornerPos;
			double tranDistance = 0;
			cMatrix3d rotMatrix;

			rotMatrix = block->getRot();


			corner = new Corner();
			corner->rotate(rotMatrix);
			//	this translation orients the corner unit at the end of the block unit
			//------------- calculations start
			B = corner->getPos();
			P = line->getVector();
			translationDistance = (block->getHeight());
			BNew = B - (translationDistance/P.length()) * P;
			//------------- calculations end
			corner->setPos(BNew);		

			//======= ROTATION 1: orienting the first corner (rotation across the line)
			cVector3d N;
			cVector3d L;

			prevLine->getVector().crossr(line->getVector(), N);
			//L = corner->getBottomSideGlobalPos() - corner->getBottomCenterGlobalPos();
			L = corner->getTopSideGlobalPos() - corner->getTopCenterGlobalPos();
			double alpha = cRadToDeg(acos((N.dot(L)/(N.length()*L.length()))));
			cVector3d normalizedLineVector;
			line->getVector().normalizer(normalizedLineVector);

			// case 0
			if(alpha >= -0.9 && alpha <=0.1)
				corner->rotate(normalizedLineVector, alpha - 90);
			// case 180
			else if(alpha >= 179.9 && alpha <=180.1)
				corner->rotate(normalizedLineVector, alpha + 90);
			// other cases
			else
			{
				cVector3d dirVector;
				L.crossr(N, dirVector);
				double dirAngle = acos((dirVector.dot(normalizedLineVector)/(dirVector.length()*normalizedLineVector.length())));
				//printf("\nDIRECTION ANGLE = %1.5f, ALPHA = %1.5f\n", cRadToDeg(dirAngle), cRadToDeg(alpha));

				// opposite direction
				if((cRadToDeg(dirAngle) <= 180.1 && cRadToDeg(dirAngle) >= 179.9))
					corner->rotate(normalizedLineVector, (-1 * alpha) - 90);
				// same direction
				else
					corner->rotate(normalizedLineVector, alpha - 90);
			}

			//======= ROTATION 2: rotating the bottom of the corner across the cross product of the two lines
			cVector3d rotAxis = cVector3d(0,0,0);
			prevLine->getVector().crossr(line->getVector(), rotAxis);
			rotAxis.normalize();
			
			//======= graphical representation of the cross product
			//cShapeLine* lin = new cShapeLine(cVector3d(0,0,0), rotAxis);
			//values->world->addChild(lin);
			//lin->setPos(line->getB());

			corner->rotateBottom(rotAxis,  -1 * theta1 - values->ROTATION_UNIT);


			//this translation orients the corner unit at the center of the block unit
			cornerPos = corner->getPos();
			tran = block->getBottomCenterGlobalPos() - corner->getTopCenterGlobalPos();
			tranDistance = tran.length();
			newCornerPos = cornerPos + tran;
			corner->setPos(newCornerPos);

			//====== corner expansion among the corner space
			Corner*	measure	= new Corner();
			measure->setPos(corner->getPos());
			measure->rotate(corner->getRot());

			
			cVector3d originalPos[6][20];
			cVector3d pos[6][20];
			cVector3d diff[6][20];
			cMatrix3d rotm[6];
			int vertNum = measure->getCylinderMesh()->getNumVertices(true);
			int faceNum = 0;

			for(int i=0; i<6; i++)
				for(int j=0; j<21; j++)
				{
					if((i==0 && j<19) || (i==1 && j<21) || (i==2 && j<21) || (i==3 && j<21) || (i==4 && j<21)
							|| (i==5 && j<20))
					originalPos[i][j] = measure->getVertexPos(i, j);
				}
			
			for(faceNum = 1; faceNum < 5; faceNum++)
			{
				if(faceNum == 1)
					rotm[faceNum] = cRotMatrix(cVector3d(1,0,0), cDegToRad(rotStep - values->ROTATION_UNIT));
				else
					rotm[faceNum] = cRotMatrix(cVector3d(1,0,0), cDegToRad(rotStep));

				for(int i=0; i<vertNum; i++)
				{
					measure->getCylinderMesh()->computeGlobalPositions();
					measure->getCylinderMesh()->pVerticesNonEmpty()->at(i).setPos(
						cMul(rotm[faceNum],measure->getCylinderMesh()->pVerticesNonEmpty()->at(i).getPos())
						);
				}


				for(int i=0; i<20; i++)
				{
					pos[faceNum][i] = measure->getVertexPos(faceNum, i);
					diff[faceNum][i] =  pos[faceNum][i] - originalPos[faceNum][i];
					corner->translateVertex(faceNum, i, diff[faceNum][i]);
				}
			
			}

			corner->recalculateCollision();
			measure->setAsGhost(true);
			measure->setPos(0, 10, 10);

			// create a magnetic line at the corner
			MagneticLine* cornerLine = 
				new MagneticLine(corner->getTopCenterGlobalPos(), corner->getBottomCenterGlobalPos());
			cornerLine->setBlock(corner);

			cornerLine->next = createdLines;
			createdLines = cornerLine;


			corner->next=cornersHead;
			cornersHead = corner; 
		}



		//------------calculations end

		//=============
		prevBlock = block;
		block->next=blocksHead;
		blocksHead = block; 

		prevLine = line;	
		line = (MagneticLine* )line->next;


		
	}

	blocksHead = block;
	linesHead = line;

	createdVFBlocks = blocksHead;
	createdCorners = cornersHead;
}

//=========================================================//

double getAngleBetweenLines(MagneticLine* prevLine, MagneticLine* line)
{
	cVector3d intersectionPoint = prevLine->getA(); //common with line->getB()
	cVector3d v1 = prevLine->getB();
	cVector3d v2 = line->getA();

	v1 = v1 - intersectionPoint;
	v2 = v2 - intersectionPoint;
	return acos((v1.dot(v2))/((v1.length())*(v2.length())));
}

//=========================================================//



