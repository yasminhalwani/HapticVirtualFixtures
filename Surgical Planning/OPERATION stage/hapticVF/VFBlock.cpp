/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Operation Stage

[VFBlock]
Serves as a structure for the cylindrical VF Blocks in the software; exhibiting 
haptic forbidden-region in the form of collisions with the mesh.
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

VFBlock::VFBlock()
{
	values = CommonValues::getInstance();

	isHidden = false;
	isGhost = false;	

	importMeshes();
	setupInitialMeshesProperties();
	scaleRadius(values->BLOCK_RADIUS_SCALE_FACTOR);
	measureInitialCylinderDimensions();
		
	scale(values->BLOCK_SCALE_FACTOR);

	collisionFlag = false;
	isCorner = false;

}

//=========================================================//

void VFBlock::importMeshes(void)
{
	string modelPath_body = "../resources/cylinder.3DS";
	string modelPath_top = "../resources/top.3DS";
	string modelPath_bottom = "../resources/bottom.3DS";

	bool fileload;
	string resourceRoot;

	cylinder = new cMesh(values->world);

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

	top = new cMesh(values->world);

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

	bottom = new cMesh(values->world);

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

void VFBlock::setupInitialMeshesProperties(void)
{
	// compute normals
    cylinder->computeAllNormals(true);	
	top->computeAllNormals(true);
	bottom->computeAllNormals(true);

    // compute a boundary box
    cylinder->computeBoundaryBox(true);
	top->computeBoundaryBox(true);
	bottom->computeBoundaryBox(true);

    // get dimensions of object
    double size = cSub(cylinder->getBoundaryMax(), cylinder->getBoundaryMin()).length();

    // resize object to screen
    cylinder->scale((2.0 * values->tool->getWorkspaceRadius() / size));
	top->scale((2.0 * values->tool->getWorkspaceRadius() / size));
	bottom->scale((2.0 * values->tool->getWorkspaceRadius() / size));

	// compute collision detection algorithm
	cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
	top->createAABBCollisionDetector(values->proxyRadius, true, false);
	bottom->createAABBCollisionDetector(values->proxyRadius, true, false);

	// setup cylinder material
	cylinderMaterial.setStiffness(0.4 * values->stiffnessMax);
	cylinder->setMaterial(cylinderMaterial, true, true);
	cylinder->setUseMaterial(true, true);

	// set the transperancy levels
	cylinder->setTransparencyLevel(values->defaultTransparencyLevel,true,true);
	top->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
	bottom->setTransparencyLevel(values->defaultTransparencyLevel, true, true);

	// set use culling for the whole object
	cylinder->setUseCulling(true,true);
	top->setUseCulling(true, true);
	bottom->setUseCulling(true, true);

	// add to world
	values->world->addChild(cylinder);
	values->world->addChild(top);
	values->world->addChild(bottom);

	// mark the side of the top mesh (top side)
	topSideSphere = new cShapeSphere(0.0001);
	top->addChild(topSideSphere);
	topSideSphere->setPos(top->pVerticesNonEmpty()->at(1).getPos());
	topSideSphere->setMaterial(values->pinkBlank);

	// mark the center of the top mesh (top center)
	topCenterSphere = new cShapeSphere(0.0001);
	top->addChild(topCenterSphere);
	topCenterSphere->setPos(top->pVerticesNonEmpty()->at(0).getPos());

	// mark the center of the bottom mesh (bottom center)
	bottomCenterSphere = new cShapeSphere(0.0001);
	bottom->addChild(bottomCenterSphere);
	bottomCenterSphere->setPos(bottom->pVerticesNonEmpty()->at(0).getPos());

	// mark the side of the bottom mesh (bottom side)
	bottomSideSphere = new cShapeSphere(0.0001);
	bottom->addChild(bottomSideSphere);
	bottomSideSphere->setPos(bottom->pVerticesNonEmpty()->at(1).getPos());
	bottomSideSphere->setMaterial(values->brownBlank);

}

//=========================================================//

void VFBlock::measureInitialCylinderDimensions(void)
{
	// calculate height
	height = cDistance(topCenterSphere->getPos(), bottomCenterSphere->getPos());

	// calculate radius
	radius = cDistance(topCenterSphere->getPos(), topSideSphere->getPos());
}

//=========================================================//

void VFBlock::setAsGhost(bool status)
{
	isGhost = status;

	if(isGhost)
	{
		cylinder->setAsGhost(true);
		top->setAsGhost(true);
		bottom->setAsGhost(true);
	}
	else
	{
		cylinder->setAsGhost(false);
		top->setAsGhost(false);
		bottom->setAsGhost(true);
	}
}

//=========================================================//

void VFBlock::setStiffnessStatus(bool status)
{
	isStiffnessEnabled = status;

	if(isStiffnessEnabled)
	{
		cylinderMaterial.setStiffness(0.4 * values->stiffnessMax);
		cylinder->setMaterial(cylinderMaterial, true, true);
		cylinder->setUseMaterial(true, true);
		cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
		cylinder->setTransparencyLevel(values->defaultTransparencyLevel,true,true);
		top->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
		bottom->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
	}
	else
	{
		cylinderMaterial.setStiffness(0);
		cylinder->setMaterial(cylinderMaterial, true, true);
		cylinder->setUseMaterial(true, true);
		cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
		cylinder->setTransparencyLevel(values->defaultTransparencyLevel,true,true);
		top->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
		bottom->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
	}
}

//=========================================================//

void VFBlock::setHide(bool status)
{
	isHidden = status;
	
	if(isHidden)
	{
		top->setTransparencyLevel(0, false, true);
		bottom->setTransparencyLevel(0, false, true);
		cylinder->setTransparencyLevel(0, false, true);
	}
	else
	{
		top->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
		bottom->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
		cylinder->setTransparencyLevel(values->defaultTransparencyLevel, true, true);
	}
}

//=========================================================//

void VFBlock::removeFromWorld(void)
{
	values->world->removeChild(top);
	values->world->removeChild(cylinder);
	values->world->removeChild(bottom);
}

//=========================================================//

void VFBlock::updateHaptics()
{
	if(collisionFlag)
	{
		if(!values->tool->isInContact(cylinder->getChild(0)))
		{
			// count the number of collisions upon each single contact
			// with the cylinder
			values->numOfCollisions++;
			values->tool->m_lastComputedGlobalForce.print();
			writeForceToFile(values->tool->m_lastComputedGlobalForce);
			collisionFlag = false;
		}
	}else
	{	// do not keep increasing number of collisions if the tool
		// did not lose contact with the cylinder yet
		if(values->tool->isInContact(cylinder->getChild(0)))
			collisionFlag = true;
	}
	

	values->tool->applyForces();
}

//=========================================================//

void VFBlock::writeForceToFile(cVector3d force)
{
	double x, y, z;

	x = force.x;
	y = force.y;
	z = force.z;

    values->outfileForces<<x<<" "<<y<<" "<<z<<endl;
}

//=========================================================//

void VFBlock::setHighlightBlockAsActive(bool status)
{
	if(status)
	{
		cylinderMaterial.m_ambient.set(0.8,0.1,0.1,1);
		cylinderMaterial.setStiffness(0.4 * values->stiffnessMax);
		cylinder->setMaterial(cylinderMaterial, true, true);
		cylinder->setUseMaterial(true, true);
		cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
		cylinder->setTransparencyLevel(values->defaultTransparencyLevel,true,true);
	}
	else
	{
		cylinderMaterial.m_ambient.set(0.0,0.0,0.0,0);
		cylinderMaterial.setStiffness(0.4 * values->stiffnessMax);
		cylinder->setMaterial(cylinderMaterial, true, true);
		cylinder->setUseMaterial(true, true);
		cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
		cylinder->setTransparencyLevel(values->defaultTransparencyLevel,true,true);
	}
}

//=========================================================//

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================TRANSFORMATIONS========================//
//=========================================================*/

void VFBlock::scale(double scaleFactor)
{
	cylinder->scale(scaleFactor);
	top->scale(scaleFactor);
	bottom->scale(scaleFactor);

	radius = radius*scaleFactor;
	height = height*scaleFactor;

	cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
	top->createAABBCollisionDetector(values->proxyRadius, true, false);
	bottom->createAABBCollisionDetector(values->proxyRadius, true, false);

	// changes the standard radius in the program in case of scaling the blocks
	values->stdBlockRadius = radius; 

}

//=========================================================//

void VFBlock::scaleHeight(double scaleFactor)
{
	cylinder->scale(cVector3d(1,1,scaleFactor), true);
	top->scale(cVector3d(1,1,scaleFactor), true);
	bottom->scale(cVector3d(1,1,scaleFactor), true);

	height = height*scaleFactor;

	cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
	top->createAABBCollisionDetector(values->proxyRadius, true, false);
	bottom->createAABBCollisionDetector(values->proxyRadius, true, false);

}

//=========================================================//

void VFBlock::scaleRadius(double scaleFactor)
{
	cylinder->scale(cVector3d(scaleFactor,scaleFactor,1), true);
	top->scale(cVector3d(scaleFactor,scaleFactor,1), true);
	bottom->scale(cVector3d(scaleFactor,scaleFactor,1), true);

	radius = radius*scaleFactor;
	values->stdBlockRadius = radius;

	cylinder->createAABBCollisionDetector(values->proxyRadius, true, false);
	top->createAABBCollisionDetector(values->proxyRadius, true, false);
	bottom->createAABBCollisionDetector(values->proxyRadius, true, false);
}

//=========================================================//

void VFBlock::translate(double x, double y, double z)
{
	cylinder->translate(x,y,z);
	top->translate(x,y,z);
	bottom->translate(x,y,z);

}

//=========================================================//

void VFBlock::translate(cVector3d translationVector)
{
	cylinder->translate(translationVector);
	top->translate(translationVector);
	bottom->translate(translationVector);

}

//=========================================================//

void VFBlock::rotate(cVector3d axisVector, double rotationAngleDegrees)
{
	cylinder->rotate(axisVector, cDegToRad(rotationAngleDegrees));
	top->rotate(axisVector, cDegToRad(rotationAngleDegrees));
	bottom->rotate(axisVector, cDegToRad(rotationAngleDegrees));

}

//=========================================================//

void VFBlock::rotate(cMatrix3d rotationMatrix)
{
	cylinder->rotate(rotationMatrix);
	top->rotate(rotationMatrix);
	bottom->rotate(rotationMatrix);

}

//=========================================================//

void VFBlock::setPos(double x, double y, double z)
{
	cylinder->setPos(x, y , z);
	top->setPos(x, y , z);
	bottom->setPos(x, y , z);
}

//=========================================================//

void VFBlock::setPos(cVector3d locationVector)
{
	cylinder->setPos(locationVector);
	top->setPos(locationVector);
	bottom->setPos(locationVector);
}

//=========================================================//

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================SETTERS AND GETTERS====================//
//=========================================================*/

void VFBlock::setDefaultTransparencyLevel(double transparencyLevel)
{
	values->defaultTransparencyLevel = transparencyLevel;
}

double VFBlock::getDefaultTransparencyLevel()
{
	return values->defaultTransparencyLevel;
}

double VFBlock::getHeight()
{
	return height;
}

double VFBlock::getRadius()
{
	return radius;
}

cVector3d VFBlock::getTopCenterGlobalPos()
{
	cVector3d position;

	bool ghostStatus = top->getAsGhost();
	if(ghostStatus==true) top->setAsGhost(false);
	values->world->computeGlobalPositions();
	top->computeGlobalPositions();
	position = topCenterSphere->getGlobalPos();	
	top->setAsGhost(ghostStatus);

	return position;
}

cVector3d VFBlock::getTopSideGlobalPos()
{
	cVector3d position;

	bool ghostStatus = top->getAsGhost();
	if(ghostStatus==true) top->setAsGhost(false);
	values->world->computeGlobalPositions();
	top->computeGlobalPositions();
	position = topSideSphere->getGlobalPos();	
	top->setAsGhost(ghostStatus);

	return position;
}

cVector3d VFBlock::getBottomCenterGlobalPos()
{
	cVector3d position;

	bool ghostStatus = bottom->getAsGhost();
	if(ghostStatus==true) bottom->setAsGhost(false);
	values->world->computeGlobalPositions();
	bottom->computeGlobalPositions();
	position = bottomCenterSphere->getGlobalPos();	
	bottom->setAsGhost(ghostStatus);

	return position;
}

cVector3d VFBlock::getBottomSideGlobalPos()
{
	cVector3d position;

	bool ghostStatus = bottom->getAsGhost();
	if(ghostStatus==true) bottom->setAsGhost(false);
	values->world->computeGlobalPositions();
	bottom->computeGlobalPositions();
	position = bottomSideSphere->getGlobalPos();	
	bottom->setAsGhost(ghostStatus);

	return position;
}

cVector3d VFBlock::getPos()
{
	return cylinder->getPos();
}

cMatrix3d VFBlock::getRot()
{
	return cylinder->getRot();
}

cMesh* VFBlock::getCylinderMesh()
{
	return cylinder;
}

cMesh* VFBlock::getTopMesh()
{
	return top;
}

cMesh* VFBlock::getBottomMesh()
{
	return bottom;
}
//=========================================================//