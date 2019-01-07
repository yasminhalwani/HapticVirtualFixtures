/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Operation Stage

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
MagneticLine*			createdLines = NULL;
VFBlock*				createdVFBlocks = NULL;
Corner*					createdCorners = NULL;

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
int						mouseX;
int						mouseY;
int						mouseButton;
int						pointsMode;
bool					flagCameraInMotion;
double					isMouseClicking;
cVector3d				toolLocalPos;
cVector3d				prevToolLocalPos;
cVector3d				cameraPosition;
cVector3d				startingPointPos;

/*=========================================================//
//=======================[PROTOTYPES]======================//
//=========================================================*/

// ---------------- program startup methods

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

// ---------------- algorithm methods - functional
// extracts the midpoints of the loaded 3D model
void					extractMidpoints(void);
// obtains the standard radius of a test block, to be applied later to all blocks
void					defineStandardRadius(void);
// adds the starting point guiding sphere to the virtual world
void					addStartingPointGuide(void);
// sets the status of the starting point, which disables or enables its magnetic effect
void					setStartingPointStatus(bool status);
// initializes the linked list of points 
void					createPoints(void);
// initializes and creates the magnetic lines connecting the midpoints
void					createMagneticLinesFromPoints(Point* pointsHead);
// initialized and creates the virtual fixture blocks encompassing the magnetic lines
void					createVFBlocksFromMagneticLines(MagneticLine* linesHead);

// ---------------- algorithm methods - print
// prints out the list of points in the points linked list
void					printPoints(Point* pointsHead);
// prints out the list of lines in the lines linked list
void					printLines(MagneticLine* linesHead);

// ---------------- algorithm methods - utilities
// gets the angle between two lines of the type of MagneticLine
double					getAngleBetweenLines(MagneticLine* prevline, MagneticLine* line);

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
	initializeScene();
	initializeHapticTool();	
	defineStandardRadius();
}

//=========================================================//

void printInstructions(void)
{
	printf ("Keyboard Options:\n\n");
    printf ("[x] - Exit application\n");
	printf ("[-] - Reduce model transparency level\n");
	printf ("[+] - Increase model transparency level\n");
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
	values->tool->m_deviceSphere->setMaterial(values->brownBlank);

	// proxy properties
    values->tool->m_proxyPointForceModel->setProxyRadius(values->proxyRadius);
    values->tool->m_proxyPointForceModel->m_collisionSettings.m_checkBothSidesOfTriangles = true;
	workspaceScaleFactor = values->tool->getWorkspaceScaleFactor();
	values->stiffnessMax = info.m_maxForceStiffness / workspaceScaleFactor;
	values->forceMax = info.m_maxForce;

	// loading the catheter tooltip shape and attaching it to the haptic tool
	// this is reused from the CHAI3D demo apps
    cMesh* drill = new cMesh(values->world);
	bool fileload;
	string resourceRoot = "../resources/drill.3ds";

    fileload = drill->loadFromFile(RESOURCE_PATH(resourceRoot));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = drill->loadFromFile(resourceRoot);
        #endif
    }
    if (!fileload)
    {
        printf("Error - 3D Model failed to load correctly.\n");
        close();
    }

    // resize tool mesh model
	drill->scale(0.001);
	const cVector3d sf = cVector3d(1,5,5);
	drill->scale(sf);

    // remove the collision detector. we do not want to compute any
    // force feedback rendering on the object itself.
    drill->deleteCollisionDetector(true);

    // define a material property for the mesh
    cMaterial mat;
    mat.m_ambient.set(0.5, 0.5, 0.5);
    mat.m_diffuse.set(0.8, 0.8, 0.8);
    mat.m_specular.set(1.0, 1.0, 1.0);
    drill->setMaterial(mat, true);
    drill->computeAllNormals(true);

    // attach drill to tool
    values->tool->m_proxyMesh->addChild(drill);
	values->tool->m_proxyMesh->setFrameSize(0.5, 0.3, true);

	// creates the tooltip sphere that is used to facilitate auto-orienting the catheter tip
	values->toolTipEndSphere = new cShapeSphere(0.001);
	values->tool->m_proxyMesh->addChild(values->toolTipEndSphere);
	values->toolTipEndSphere->setPos(values->tool->m_proxyMesh->pVerticesNonEmpty()->at(0).getPos());
	values->toolTipOriginalOrientation = values->tool->m_proxyMesh->getRot();
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

	if(key=='-')
	{	
		modelTransparency-=0.1;
		model->setTransparencyLevel(modelTransparency);
	}

	if(key=='+')
	{
		modelTransparency+=0.1;
		model->setTransparencyLevel(modelTransparency);
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

    cThread* hapticsThread = new cThread();
    hapticsThread->set(updateHaptics, CHAI_THREAD_PRIORITY_HAPTICS);

    glutMainLoop();

    close();
}

//=========================================================//

void updateHaptics(void)
{
	// initializes the linked lists
	MagneticLine* tempLines = createdLines;
	VFBlock* tempBlocks = createdVFBlocks;
	Corner* tempCorners = createdCorners;

    while(simulationRunning)
    {
		values->world->computeGlobalPositions(true);
		values->tool->updatePose();
		values->tool->computeInteractionForces();

		toolLocalPos  = values->tool->getDeviceLocalPos();

		// the tooltip sphere moves along with the tool
		values->toolTipEndSphere->setPos(values->tool->m_proxyMesh->pVerticesNonEmpty()->at(0).getPos());

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

		prevToolLocalPos  = toolLocalPos;

		// iterates over the linked list members to call its updateHaptics method
		tempLines = createdLines;
		while( tempLines!=NULL )
		{
			tempLines->updateHaptics();
			tempLines = tempLines->next;   
		}

		// iterates over the linked list members to call its updateHaptics method
		tempBlocks = createdVFBlocks;
		while( tempBlocks!=NULL )
		{
			tempBlocks->updateHaptics();
			tempBlocks = tempBlocks->next;   
		}

		// iterates over the linked list members to call its updateHaptics method
		tempCorners = createdCorners;
		while( tempCorners!=NULL )
		{
			tempCorners->updateHaptics();
			tempCorners = tempCorners->next;   
		}

		// determines the activation/deactivation of the starting sphere according to the location of the tool
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

void defineStandardRadius(void)
{
	VFBlock* testBlock = new VFBlock();
	values->stdBlockRadius = testBlock->getRadius();
	values->stdBlockHeight = testBlock->getHeight();
	testBlock->removeFromWorld(); // remove it from the scene so it won't interfere with the model
}

//=========================================================//

void createPoints()
{
	Point* pointsHead = NULL;
	Point* point;
	
	int numOfMidpoints = 0;
	point = (Point*)malloc(sizeof(Point)); 
	string line;
	double x;
	double y;
	double z;
	ifstream myfile ("points.txt");
	// parses the x,y,z of each point in the external txt file
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			numOfMidpoints++;
			string delimiter = ",";

			size_t pos = 0;
			std::string token;
			for(int i=0 ; i<2; i++)
			{
				pos = line.find(delimiter);
				token = line.substr(0, pos);
				if(i==0)
					x = atof(token.c_str());
				if(i==1)
					y = atof(token.c_str());

				line.erase(0, pos + delimiter.length());
			}
			z = atof(line.c_str());
			point = new Point(cVector3d(x,y,z));
			point->next=pointsHead; // point is added to the linked list of midpoints
			pointsHead = point; 
					
		}
		myfile.close();

		createdPoints = pointsHead;

		startingPointPos = createdPoints->point;

		addStartingPointGuide(); // the starting point guide is the first point in the linked list

		values->numOfMidPoints = numOfMidpoints; 
	}
			
	else {
		cout << "Unable to open file"; 
		close();
		exit(0); // close the program if the midpoints were not loaded
	}

	
}

//=========================================================//

void addStartingPointGuide()
{
	// creates a sphere with a 0.03 radius
	startingPoint = new cShapeSphere(0.03);

	// adds it to the world
	values->world->addChild(startingPoint);

	// sets its position 
	startingPoint->setPos(startingPointPos);

	// initializes the magnetic material of the sphere
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
		// sets the starting point sphere as effective
		startingPoint->setAsGhost(false);
	}
	else
	{
		// disables the effect of the starting point sphere
		startingPoint->setAsGhost(true);
	}
	
}

//=========================================================//

void extractMidpoints()
{
	// Documentation: [Methods: 01 - Obtaining midpoints by averaging]

	Point* pointsHead = NULL;
	Point* point;
	point = (Point*)malloc(sizeof(Point)); 

	int vertNum = model->getNumVertices(true);
	int space = 0.1 * vertNum;
	int numOfLoops = vertNum / space;
	values->numOfMidPoints = numOfLoops;
	cVector3d sum;
	int min, max;

	model->computeGlobalPositions();

	for(int j = 0; j<numOfLoops; j++)
	{
		min = j * space;
		max = min + space;

		sum = model->pVerticesNonEmpty()->at(min).getPos();
		for(int i=min; i<max; i++)
		{
			sum = sum + model->pVerticesNonEmpty()->at(i).getPos();
		}
		
		cVector3d avg = sum / space;

		cShapeSphere* x = new cShapeSphere(0.005); 
		values->world->addChild(x);
		x->setPos(avg);

		point = new Point(avg);
		point->next=pointsHead;
		pointsHead = point; 
	}

	createdPoints = pointsHead;

	startingPointPos = createdPoints->point;
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

	// this loop iterates over all the midpoints
	for(int i=0; i<values->numOfMidPoints-1; i++)
	{
		// creates a line between each point and the next
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
	// created to traverse the magnetic lines linked list
	MagneticLine*	line		= linesHead;
	// assumes the initial magnetic line is set to (0,0,0)
	MagneticLine*	prevLine	= new MagneticLine(cVector3d(0,0,0), cVector3d(0,0,0));

	// VF Blocks linked list
	VFBlock*		prevBlock	= NULL;
	VFBlock*		blocksHead	= NULL;
	VFBlock*		block		= (VFBlock*)malloc(sizeof(VFBlock));

	// Corners linked list
	Corner*			cornersHead	= NULL;
	Corner*			corner		= (Corner*)malloc(sizeof(Corner));

	// other necessary variables for the algorithms in this method
	int				lineCount	= 0;
	double			vectorLength;
	double			angle;
	cVector3d		v1;
	cVector3d		v2;	
	cVector3d		axis;
	cVector3d		B;
	cVector3d		P;
	cVector3d		BNew;
	cQuaternion		quat;
	cMatrix3d		rotMatrix;
	double			translationDistance;
	double			theta, compTheta, radius, theta1, theta2, theta3, theta4, n, x, sf2, sf2prev, rotStep;
	double			directionAngle;
		
	while(line!=NULL)
		// this is the main loop that traverses the created magnetic lines
	{
		lineCount++;

		// get the angle between the two lines
		v2 = line->getVector();
		vectorLength = v2.length();
		theta = cRadToDeg(getAngleBetweenLines(prevLine, line));

		//---------------- create and orient the cylinder along the direction vector
		// Documentation: [Methods: 04 - Orienting the cylinder along a vector]
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

		block->scaleHeight(vectorLength / values->stdBlockHeight);
		block->setPos(line->getA());
		line->setBlock(block);
	
		//translate cylinder across direction vector
		// Documentation: [Methods: 05 - Translating the cylinder across a vector]
		B = block->getPos();
		P = line->getVector();
		translationDistance = (block->getHeight()/2);
		BNew = B + (translationDistance/P.length()) * P;
		block->setPos(BNew);	


		//----------------------------------------remove extra height blocking the rotation units
		// Documentation: [Methods: 06 - Removing extra cylinder parts at the corners]
		radius = values->stdBlockRadius;

		compTheta	= 360 - theta;
		theta1		= compTheta - 180;							
		theta2		= (180 - theta1) / 2;	
		n			= (radius * sin(cDegToRad(theta1))) / (sin(cDegToRad(theta2)));
		theta3		= 90 - theta2;								
		theta4		= 180 - (2 * theta3);
		x			= (n * sin(cDegToRad(theta3))) / (sin(cDegToRad(theta4)));
		rotStep		= -1 * theta1/4;
		
		sf2 = 1 - (x/block->getHeight());

		if(lineCount>1)sf2prev = 1 - (x/prevBlock->getHeight());

		// if 2 or more lines exist, remove the extra cylinder parts and start creating corners
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

			// Documentation: [Methods: 07 - Orienting the corner]
			// creates a new corner
			corner = new Corner();
			corner->rotate(rotMatrix);
			//	this translation orients the corner unit at the end of the block unit		
			B = corner->getPos();
			P = line->getVector();
			translationDistance = (block->getHeight());
			BNew = B - (translationDistance/P.length()) * P;
			corner->setPos(BNew);		

			//======= ROTATION 1: orienting the corner (rotation across the line)
			cVector3d N;
			cVector3d L;

			prevLine->getVector().crossr(line->getVector(), N);
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

			corner->rotateBottom(rotAxis,  -1 * theta1 - values->ROTATION_UNIT);

			//this translation orients the corner unit at the center of the block unit
			cornerPos = corner->getPos();
			tran = block->getBottomCenterGlobalPos() - corner->getTopCenterGlobalPos();
			tranDistance = tran.length();
			newCornerPos = cornerPos + tran;
			corner->setPos(newCornerPos);

			//====== corner expansion among the corner space
			// Documentation: [Methods: 08 - Corner expansion]
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
			// the magnetic line is created as a connecting line between the center of the entrance
			// and the center of the exit of the corner
			MagneticLine* cornerLine = 
				new MagneticLine(corner->getTopCenterGlobalPos(), corner->getBottomCenterGlobalPos());
			cornerLine->setBlock(corner);

			// the line is then added to the linked list of magnetic lines
			cornerLine->next = createdLines;
			createdLines = cornerLine;

			corner->next=cornersHead;
			cornersHead = corner; 

		} // end of corner creation


		// create the next block
		prevBlock = block;
		block->next=blocksHead;
		blocksHead = block; 

		// move to the next line
		prevLine = line;	
		line = (MagneticLine* )line->next;


		
	} // end of main loop over the magnetic lines

	blocksHead = block;
	linesHead = line;

	createdVFBlocks = blocksHead;
	createdCorners = cornersHead;
}

//=========================================================//

double getAngleBetweenLines(MagneticLine* prevLine, MagneticLine* line)
{
	// Documentation: [Methods: 03 - Angle between two lines]
	cVector3d intersectionPoint = prevLine->getA(); //common with line->getB()
	cVector3d v1 = prevLine->getB();
	cVector3d v2 = line->getA();

	v1 = v1 - intersectionPoint;
	v2 = v2 - intersectionPoint;
	return acos((v1.dot(v2))/((v1.length())*(v2.length())));
}

//=========================================================//



