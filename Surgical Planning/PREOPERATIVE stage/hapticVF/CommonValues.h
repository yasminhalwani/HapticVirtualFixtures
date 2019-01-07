/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Preoperative Stage

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
	double					forceMax;
	cWorld*					world;
	cGeneric3dofPointer*	tool;	

	//========================[METHODS]========================//
	//returns the unique instance of CommmonValues
    static CommonValues* getInstance(void);	
	//destructor
    ~CommonValues(void);
	
};