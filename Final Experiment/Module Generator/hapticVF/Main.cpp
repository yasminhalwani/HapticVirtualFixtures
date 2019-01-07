/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Final Experiment - Module Generator

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

// ---------------- other numeric values
double					windowSizeW;
double					windowSizeH;
double					displayW;
double					displayH;
double					workspaceScaleFactor;
double					simulationRunning;
double					simulationFinished;      
bool					flagCameraInMotion;
int						mouseX;
int						mouseY;
int						mouseButton;
double					isMouseClicking;
cVector3d				toolLocalPos;
cVector3d				prevToolLocalPos;
cVector3d				cameraPosition;
int						pointsMode;
const int				LOADED_MODEL		= 0;
const int				PREDEFINED_POINTS	= 1;
const int				READ_FROM_FILE		= 2;

// ---------------- linked lists
Point*					createdPoints = NULL;
MagneticLine*			createdLines = NULL;
VFBlock*				createdVFBlocks = NULL;
Corner*					createdCorners = NULL;

cVector3d startingPointPos;

/*=========================================================//
//=======================[PROTOTYPES]======================//
//=========================================================*/

// ---------------- program startup methods
void					setupEnvironment(void);
void					initializeValues(void);
void					printInstructions(void);
void					initializeScene(void);
void					loadModel(void);
void					extractMidpoints(void);
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

#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

int main(int argc, char* argv[])
{		
	pointsMode = READ_FROM_FILE;

	values = CommonValues::getInstance();

	values->isTutorialModule = true;
	//values->V = false;
	//values->G = false;
	//values->F = false;

	values->moduleName = "TUTORIAL";

	setupEnvironment();
	setupGlutSettings(argc, argv);
 
	if(pointsMode == LOADED_MODEL)loadModel();
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

	windowSizeW = 1000;
	windowSizeH = 1000;
	workspaceScaleFactor = 0.0;
	simulationRunning = false;
	simulationFinished = false;
	displayW = 0;
	displayH = 0;
}

//=========================================================//

void printInstructions(void)
{
	cout<<"Module ["<<values->moduleName<<"]"<<endl;
	if(!values->isTutorialModule){
		cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
		cout<<"Please move through the path 3 times, each time with a different view.\n";
		cout<<"In case of errors, press [r] to repeat the module.\n";
		cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
		cout<<"Trial #"<<values->trials<<endl;
	}else
	{
		cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
		cout<<"You can change the camera orientation with:\n[1] The haptic middle switch, or"<<endl;
		cout<<"[2] The mouse: \n  Left click + drag: rotates across X and Y\n  Right click + drag: moves across Z (depth)"<<endl;
		cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	}
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

	values->cameraAngleH = 0;
    values->cameraAngleV = 45;
	values->cameraDistance = 3;
    updateCameraPosition();
	isMouseClicking = false;
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
    values->tool->setPos(-values->cameraDistance, 0.0, 0.0);
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

	if(values->F)values->cylinderStiffness = 0.4 * values->stiffnessMax;
	else values->cylinderStiffness = 0;


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
	drill->scale(0.002);
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

	values->toolTipEndSphere = new cShapeSphere(0.001);
	values->tool->m_proxyMesh->addChild(values->toolTipEndSphere);
	values->toolTipEndSphere->setPos(values->tool->m_proxyMesh->pVerticesNonEmpty()->at(0).getPos());

	values->toolTipOriginalOrientation = values->tool->m_proxyMesh->getRot();
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
    int windowPosX = (screenW - windowSizeW);
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
		printf("Device Global Pos: ");
		values->tool->getDeviceGlobalPos().print();
	}

	if(key=='r')
	{	
		if(!values->isTutorialModule){
			values->trials = 1;
			cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
			printf("Number of trials is reset\n");
			cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
			printf("Trial # %d\n", values->trials);
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
		if (mouseButton == GLUT_RIGHT_BUTTON)
		{
			values->cameraDistance = values->cameraDistance - 0.01 * (y - mouseY);
		}

        else if (mouseButton == GLUT_LEFT_BUTTON)
        {
            values->cameraAngleH = values->cameraAngleH - (x - mouseX);
            values->cameraAngleV = values->cameraAngleV + (y - mouseY);
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
    if (values->cameraDistance < 0.1) { values->cameraDistance = 0.1; }
    if (values->cameraAngleV > 89) { values->cameraAngleV = 89; }
    if (values->cameraAngleV < -89) { values->cameraAngleV = -89; }

	 // compute position of camera in space
    cVector3d pos = cAdd(
                        cameraPosition,
                        cVector3d(
                            values->cameraDistance * cCosDeg(values->cameraAngleH) * cCosDeg(values->cameraAngleV),
                            values->cameraDistance * cSinDeg(values->cameraAngleH) * cCosDeg(values->cameraAngleV),
                            values->cameraDistance * cSinDeg(values->cameraAngleV)
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
    values->tool->setPos(-values->cameraDistance, 0.0, 0.0);
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

		values->toolTipEndSphere->setPos(values->tool->m_proxyMesh->pVerticesNonEmpty()->at(0).getPos());


		if(values->tool->getUserSwitch(0))
		{
			 // compute tool offset
            cVector3d offset = toolLocalPos - prevToolLocalPos;

            // apply camera motion
            values->cameraDistance = values->cameraDistance - 2 * offset.x;
            values->cameraAngleH = values->cameraAngleH - 40 * offset.y;
            values->cameraAngleV = values->cameraAngleV - 40 * offset.z;  
		}

		updateCameraPosition(); 

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

		if(values->hasModuleEnded)close();

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
	string modelPath = "../resources/torusknot.obj";

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

	cMaterial modelMaterial;

    model->computeAllNormals(true);	
    model->computeBoundaryBox(true);
    double size = cSub(model->getBoundaryMax(), model->getBoundaryMin()).length();
    model->scale((2.0 * values->tool->getWorkspaceRadius() / size));
	//model->createAABBCollisionDetector(values->proxyRadius, true, false);
	//modelMaterial.setStiffness(0.4 * values->stiffnessMax);
	//model->setMaterial(modelMaterial, true, true);
	//model->setUseMaterial(true, true);
	model->setAsGhost(true);
	model->setTransparencyLevel(1,true,true);
	model->setUseCulling(true,true);
	model->setTransparencyLevel(0.4);
	values->world->addChild(model);
}

//=========================================================//

void createPoints()
{
	const int POINTS_COUNT = 6;
	Point* pointsHead = NULL;
	Point* point;
	values->numOfMidPoints = POINTS_COUNT;
	//================================ POINTS ARRAY
	cVector3d pointsArray[POINTS_COUNT]; 
	switch(pointsMode)
	{
		case PREDEFINED_POINTS:		
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
			point = (Point*)malloc(sizeof(Point)); 

			for(int i=0; i<POINTS_COUNT; i++)
			{
				point = new Point(pointsArray[i]);
				point->next=pointsHead;
				pointsHead = point; 
			}

			createdPoints = pointsHead;
			startingPointPos = pointsArray[POINTS_COUNT -1];
			addStartingPointGuide();
			break;
		case LOADED_MODEL:
			extractMidpoints();
			addStartingPointGuide();
			break;
		case READ_FROM_FILE:
			int numOfMidpoints = 0;
			point = (Point*)malloc(sizeof(Point)); 
			string line;
			double x;
			double y;
			double z;
			ifstream myfile ("points_tutorial.txt");
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
					point->next=pointsHead;
					pointsHead = point; 
					
				}
				myfile.close();

				createdPoints = pointsHead;

				startingPointPos = createdPoints->point;

				addStartingPointGuide();

				values->numOfMidPoints = numOfMidpoints;
			}
			
			else {
				cout << "Unable to open file"; 
				close();
				exit(0);
			}
			break;
	}

	
}

//=========================================================//

void extractMidpoints()
{
	Point* pointsHead = NULL;
	Point* point;

	point = (Point*)malloc(sizeof(Point)); 

	int vertNum = model->getNumVertices(true);
	int space = 50;


	int numOfLoops = vertNum / space;
	values->numOfMidPoints = numOfLoops;
	printf("num vertices = %d\n", vertNum);

	cVector3d sum;

	model->computeGlobalPositions();

	int min, max;


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

void addStartingPointGuide()
{
	startingPoint = new cShapeSphere(0.03);

	values->world->addChild(startingPoint);

	startingPoint->setPos(startingPointPos);

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
	for(int i=0; i<values->numOfMidPoints-1; i++)
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



