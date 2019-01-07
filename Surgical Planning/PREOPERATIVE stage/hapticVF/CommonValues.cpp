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

/*=========================================================//
//========================[VARIABLES]======================//
//=========================================================*/
bool			CommonValues::instanceFlag					= false;
CommonValues*	CommonValues::single						= NULL;


/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//=========================================================*/

CommonValues::CommonValues(void)
{
	proxyRadius				= 0;
	workspaceScaleFactor	= 0;
	stiffnessMax			= 0;
	forceMax				= 0;
	world					= NULL;
	tool					= NULL;

}

//=========================================================//
CommonValues* CommonValues::getInstance(void)
{
    if(! instanceFlag)
    {
        single = new CommonValues();
        instanceFlag = true;
        return single;
    }
    else
    {
        return single;
    }
}

//=========================================================//

CommonValues::~CommonValues(void)
{
    instanceFlag = false;
}

//=========================================================//
