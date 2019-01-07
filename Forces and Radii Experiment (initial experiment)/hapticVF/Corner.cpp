/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Initial Experiment 

[Corner]
Serves as a structure for the corners in the software; inheriting all the 
attributes and methods of VFBlock with some more changes.
Built for linked lists.

For more details, please refer to the documentation.

Developed by Yasmin Halwani		(yasmin.halwani@outlook.com)
Supervised by Dr. Osama Halabi	(ohalabi@qu.edu.qa)
Computer Science and Engineering Department
Qatar University
2014
****************************************************************************/

#include "stdafx.h"


/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//=========================================================*/

Corner::Corner()
{
	redefineMeshes();
	setupInitialMeshesProperties();
	scaleRadius(values->BLOCK_RADIUS_SCALE_FACTOR);
	measureInitialCylinderDimensions();
	modifySize();

	cylinder->setFrameSize(0.5, 0.2, true);

}

//=========================================================//

void Corner::redefineMeshes(void)
{	
	string modelPath_body = "../resources/corner_noCovers.3DS";
	string modelPath_top = "../resources/corner_top.3DS";
	string modelPath_bottom = "../resources/corner_bottom.3DS";

	bool fileload;
	string resourceRoot;
	
	cylinder->deleteAllChildren();

	fileload = cylinder->loadFromFile(RESOURCE_PATH(modelPath_body));
	if (!fileload)
	{
		#if defined(_MSVC)
		fileload = cylinder->loadFromFile(modelPath_body);
		#endif
	}
	if (!fileload)
	{
		printf("Error - 3D Model [body] failed to load correctly.\n");
	}

	//===================================================

	top->deleteAllChildren();

	fileload = top->loadFromFile(RESOURCE_PATH(modelPath_top));
	if (!fileload)
	{
		#if defined(_MSVC)
		fileload = top->loadFromFile(modelPath_top);
		#endif
	}
	if (!fileload)
	{
		printf("Error - 3D Model [top] failed to load correctly.\n");
	}

	//===================================================

	bottom->deleteAllChildren();

	fileload = bottom->loadFromFile(RESOURCE_PATH(modelPath_bottom));
	if (!fileload)
	{
		#if defined(_MSVC)
		fileload = bottom->loadFromFile(modelPath_bottom);
		#endif
	}
	if (!fileload)
	{
		printf("Error - 3D Model [bottom] failed to load correctly.\n");
	}
}

//=========================================================//

void Corner::modifySize(void)
{
	cylinder->scale(values->CORNER_SCALE_FACTOR);
	top->scale(values->CORNER_SCALE_FACTOR);
	bottom->scale(values->CORNER_SCALE_FACTOR);	

	scale(values->BLOCK_SCALE_FACTOR);
}

//=========================================================//

void Corner::setVertexPos(int faceNum, int vertexNum, cVector3d pos)
{
	cylinder->pVerticesNonEmpty()->at(values->vertexIndex[faceNum][vertexNum]).setPos(pos);
}

//=========================================================//

cVector3d Corner::getVertexPos(int faceNum, int vertexNum)
{
	cylinder->computeGlobalPositions();
	return cylinder->pVerticesNonEmpty()->at(values->vertexIndex[faceNum][vertexNum]).getPos();
}

//=========================================================//

void Corner::translateVertex(int faceNum, int vertexNum, cVector3d tran)
{
	cylinder->pVerticesNonEmpty()->at(values->vertexIndex[faceNum][vertexNum]).translate(tran);
}

//=========================================================//

void Corner::recalculateCollision(void)
{
	cylinder->createAABBCollisionDetector(values->proxyRadius, true, true);
	cylinder->computeAllNormals(true);
	cylinder->computeBoundaryBox(true);

	top->createAABBCollisionDetector(values->proxyRadius, true, true);
	top->computeAllNormals(true);
	top->computeBoundaryBox(true);

	bottom->createAABBCollisionDetector(values->proxyRadius, true, true);
	bottom->computeAllNormals(true);
	bottom->computeBoundaryBox(true);
}

//=========================================================//

void Corner::rotateBottom(cVector3d rotAxis, double rotAngleDegrees)
{
	bottom->rotate(rotAxis, cDegToRad(rotAngleDegrees));
	bottomCenterSphere->rotate(rotAxis, cDegToRad(rotAngleDegrees));
	bottomSideSphere->rotate(rotAxis, cDegToRad(rotAngleDegrees));
}

//=========================================================//

