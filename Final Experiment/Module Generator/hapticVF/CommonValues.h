/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Final Experiment - Module Generator

[Common Values]
This is a singleton class that serves as a container for the global variables
used in multiple classes in the program.

For more details, please refer to the documentation.

Developed by Yasmin Halwani		(yasmin.halwani@outlook.com)
Supervised by Dr. Osama Halabi	(ohalabi@qu.edu.qa)
Computer Science and Engineering Department
Qatar University
2014
****************************************************************************/

#include "stdafx.h"

class CommonValues
{
/*=========================================================//
//========================[PRIVATE]========================//
//=========================================================*/
private:
	//========================[VARIABLES]======================//
    static bool			instanceFlag;
    static CommonValues *single;

	//========================[METHODS]========================//
	//constructor
	CommonValues(void);
	// initializes the materials 
	void initializeMaterials(void);

/*=========================================================//
//========================[PUBLIC]=========================//
//=========================================================*/
public:
	//========================[VARIABLES]======================//
	double					proxyRadius;
	double					workspaceScaleFactor;
	double					stiffnessMax;
	double					cylinderStiffness;
	double					forceMax;
	cWorld*					world;
	cGeneric3dofPointer*	tool;	
	Point*					createdPoints;
	cMaterial				pinkBlank;
	cMaterial				brownBlank;
	cMaterial				magneticSphereMat;
	double					stdBlockRadius;
	double					stdBlockHeight;
	int						vertexIndex[6][21]; //face, vertex number
	int						numOfCollisions;
	double					forceScaleFactor;
	double					defaultTransparencyLevel;
	bool					isInsideThePath;
	time_t					startingTime;
	time_t					endingTime;
	double					totalTime1;
	double					totalTime2;
	double					totalTime3;
	double					averageTime;
	double					startingNumCollisions;
	double					endingNumCollisions;
	double					totalNumCollisions1;
	double					totalNumCollisions2;
	double					totalNumCollisions3;
	double					averageTotalNumCollisions;
	ofstream				outfileForces;
	ofstream				outfileCollisionTime;
	double					numOfMidPoints;
	cShapeSphere*			toolTipEndSphere;
	cMatrix3d				toolTipOriginalOrientation;
	double					cameraAngleH;
	double					cameraAngleV;
	double					cameraDistance; 
	bool					hasModuleEnded;
	bool					isTutorialModule;
	
	//========================[EXPERIMENT'S VARIABLES]=========//
	bool					V;
	bool					F;
	bool					G;
	int						trials;
	string					moduleName;


	//========================[CONSTANTS]======================//
	static const double CORNER_SCALE_FACTOR;
	static const double BLOCK_SCALE_FACTOR;
	static const double BLOCK_RADIUS_SCALE_FACTOR;
	static const double ROTATION_UNIT;


	//========================[METHODS]========================//
	//returns the unique instance of CommmonValues
    static CommonValues* getInstance(void);	
	//destructor
    ~CommonValues(void);
	
};