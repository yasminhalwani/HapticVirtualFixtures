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

/*=========================================================//
//========================[VARIABLES]======================//
//=========================================================*/
bool			CommonValues::instanceFlag					= false;
CommonValues*	CommonValues::single						= NULL;
const double	CommonValues::CORNER_SCALE_FACTOR			= 0.8155; // this value is experimental, keep it this way for this mesh
const double	CommonValues::BLOCK_SCALE_FACTOR			= 0.2;
const double	CommonValues::ROTATION_UNIT					= -6.0;
const double	CommonValues::BLOCK_RADIUS_SCALE_FACTOR		= 0.7; // use this to change the radius of all blocks and corners


/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//=========================================================*/

CommonValues::CommonValues(void)
{
	isInsideThePath = false;

	proxyRadius				= 0;
	workspaceScaleFactor	= 0;
	stiffnessMax			= 0;
	forceMax				= 0;
	world					= NULL;
	tool					= NULL;
	numOfCollisions			= 0;
	forceScaleFactor		= 1;
	defaultTransparencyLevel= 0.5;

	initializeMaterials();

	// Note that these numbers are specific to the mesh that comes with this program
	// Do NOT use any other corner mesh, as these number will not be correct and the 
	// mesh will fail to expand correctly at the corners

	//FACE 0
	vertexIndex[0][0] = 102;
	vertexIndex[0][1] = 103;
	vertexIndex[0][2] = 104;
	vertexIndex[0][3] = 105;
	vertexIndex[0][4] = 106;
	vertexIndex[0][5] = 108;
	vertexIndex[0][6] = 109;
	vertexIndex[0][7] = 111;
	vertexIndex[0][8] = 112;
	vertexIndex[0][9] = 113;
	vertexIndex[0][10]= 115; 
	vertexIndex[0][11]= 116;
	vertexIndex[0][12]= 117;
	vertexIndex[0][13]= 118;
	vertexIndex[0][14]= 119;
	vertexIndex[0][15]= 120;
	vertexIndex[0][16]= 121;
	vertexIndex[0][17]= 122;
	vertexIndex[0][18]= 114;

	//FACE 1
	vertexIndex[1][0] = 81;
	vertexIndex[1][1] = 82;
	vertexIndex[1][2] = 83;
	vertexIndex[1][3] = 84;
	vertexIndex[1][4] = 85;
	vertexIndex[1][5] = 87;
	vertexIndex[1][6] = 88;
	vertexIndex[1][7] = 90;
	vertexIndex[1][8] = 91;
	vertexIndex[1][9] = 92;
	vertexIndex[1][10]= 94;
	vertexIndex[1][11]= 95;
	vertexIndex[1][12]= 96;
	vertexIndex[1][13]= 97;
	vertexIndex[1][14]= 98;
	vertexIndex[1][15]= 99;
	vertexIndex[1][16]= 100;
	vertexIndex[1][17]= 101;
	vertexIndex[1][18]= 107;
	vertexIndex[1][19]= 110;
	vertexIndex[1][20]= 93;

	//FACE 2
	vertexIndex[2][0] = 60;
	vertexIndex[2][1] = 61;
	vertexIndex[2][2] = 62;
	vertexIndex[2][3] = 63;
	vertexIndex[2][4] = 64;
	vertexIndex[2][5] = 66;
	vertexIndex[2][6] = 67;
	vertexIndex[2][7] = 69;
	vertexIndex[2][8] = 70;
	vertexIndex[2][9] = 71;
	vertexIndex[2][10]= 73;
	vertexIndex[2][11]= 74;
	vertexIndex[2][12]= 75;
	vertexIndex[2][13]= 76;
	vertexIndex[2][14]= 77;
	vertexIndex[2][15]= 78;
	vertexIndex[2][16]= 79;
	vertexIndex[2][17]= 80;
	vertexIndex[2][18]= 86;
	vertexIndex[2][19]= 89;
	vertexIndex[2][20]= 72;

	//FACE 3
	vertexIndex[3][0] = 39;
	vertexIndex[3][1] = 40;
	vertexIndex[3][2] = 41;
	vertexIndex[3][3] = 42;
	vertexIndex[3][4] = 43;
	vertexIndex[3][5] = 45;
	vertexIndex[3][6] = 46;
	vertexIndex[3][7] = 48;
	vertexIndex[3][8] = 49;
	vertexIndex[3][9] = 50;
	vertexIndex[3][10]= 52;
	vertexIndex[3][11]= 53;
	vertexIndex[3][12]= 54;
	vertexIndex[3][13]= 55;
	vertexIndex[3][14]= 56;
	vertexIndex[3][15]= 57;
	vertexIndex[3][16]= 58;
	vertexIndex[3][17]= 59;
	vertexIndex[3][18]= 65;
	vertexIndex[3][19]= 68;
	vertexIndex[3][20]= 51;

	//FACE 4
	vertexIndex[4][0] = 1;
	vertexIndex[4][1] = 2;
	vertexIndex[4][2] = 4;
	vertexIndex[4][3] = 6;
	vertexIndex[4][4] = 8;
	vertexIndex[4][5] = 11;
	vertexIndex[4][6] = 12;
	vertexIndex[4][7] = 15;
	vertexIndex[4][8] = 17;
	vertexIndex[4][9] = 19;
	vertexIndex[4][10]= 23;
	vertexIndex[4][11]= 25;
	vertexIndex[4][12]= 27;
	vertexIndex[4][13]= 29;
	vertexIndex[4][14]= 31;
	vertexIndex[4][15]= 33;
	vertexIndex[4][16]= 35;
	vertexIndex[4][17]= 37;
	vertexIndex[4][18]= 44;
	vertexIndex[4][19]= 47;
	vertexIndex[4][20]= 21;

	//FACE 5
	vertexIndex[5][0] = 0;
	vertexIndex[5][1] = 3;
	vertexIndex[5][2] = 5;
	vertexIndex[5][3] = 7;
	vertexIndex[5][4] = 9;
	vertexIndex[5][5] = 10;
	vertexIndex[5][6] = 13;
	vertexIndex[5][7] = 14;
	vertexIndex[5][8] = 16;
	vertexIndex[5][9] = 18;
	vertexIndex[5][10]= 20;
	vertexIndex[5][11]= 24;
	vertexIndex[5][12]= 26;
	vertexIndex[5][13]= 28;
	vertexIndex[5][14]= 30;
	vertexIndex[5][15]= 32;
	vertexIndex[5][16]= 34;
	vertexIndex[5][17]= 36;
	vertexIndex[5][18]= 38;
	vertexIndex[5][19]= 22;
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

void CommonValues::initializeMaterials(void)
{
	pinkBlank.m_ambient.set(0.8, 0.1, 0.4);
	pinkBlank.m_diffuse.set(1.0, 0.15, 0.5);
	pinkBlank.m_specular.set(1.0, 0.2, 0.8);

	brownBlank.m_ambient.set(1.0, 0.1, 0.5);
	brownBlank.m_diffuse.set(0.5, 0.8, 0.3);
	brownBlank.m_specular.set(0.4, 0.8, 0.1);

	magneticSphereMat.m_ambient.set(1.0, 0.5, 0.5);
	magneticSphereMat.m_diffuse.set(1.0, 0.3, 0.3);
	magneticSphereMat.m_specular.set(1.0, 0.3, 0.3);
}

//=========================================================//