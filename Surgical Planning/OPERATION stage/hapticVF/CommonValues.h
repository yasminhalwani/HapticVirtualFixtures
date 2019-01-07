/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Operation Stage

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
	// initializes the colors of the shared materials 
	void initializeMaterials(void);

/*=========================================================//
//========================[PUBLIC]=========================//
//=========================================================*/
public:
	//========================[VARIABLES]======================//
	double					proxyRadius;
	double					workspaceScaleFactor;
	double					stiffnessMax;
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
	double					totalTime;
	double					startingNumCollisions;
	double					endingNumCollisions;
	double					totalNumCollisions;
	ofstream				outfileForces;
	ofstream				outfileCollisionTime;
	double					numOfMidPoints;
	cShapeSphere*			toolTipEndSphere; // this sphere is used to auto-orient the catheter tool inside the cylinders
	cMatrix3d				toolTipOriginalOrientation;

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