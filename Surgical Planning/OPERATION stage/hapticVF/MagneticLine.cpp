/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Operation Stage

[MagneticLine]
Serves as a structure for the magnetic lines in the software; exhibiting haptic
guiding force fields.
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

MagneticLine::MagneticLine(cVector3d A, cVector3d B)
{
	values = CommonValues::getInstance();

	this->A = A;
	this->B = B;
	vector = B - A;
	lineShape = new cShapeLine(A, B);
	lineShape->m_ColorPointA.set(1, 0, 0);
	lineShape->m_ColorPointB.set(1, 0, 0);

	this->isSecondLine = false;
	this->isLastLine = false;

	this->isToolOriented = false;
	this->orientationCount = 0;
}

//=========================================================//

MagneticLine::MagneticLine(void)
{
	values = CommonValues::getInstance();
	
	A = cVector3d(0,0,0);
	B = cVector3d(0,0,0);
	vector = cVector3d(0,0,0);
	lineShape = new cShapeLine(A, B);
	lineShape->m_ColorPointA.set(1, 0, 0);
	lineShape->m_ColorPointB.set(1, 0, 0);

	this->isSecondLine = false;
	this->isLastLine = false;

	this->isToolOriented = false;
	this->orientationCount = 0;
}

//=========================================================//

void MagneticLine::print(void)
{
	printf("---- Line ---- \n");
	printf("A: ");
	A.print();
	printf("B: ");
	B.print();
	printf("V: ");
	vector.print();
	printf("L: ");
	printf("%1.2f", vector.length());
	printf("\n");
	printf("--------------");
	printf("\n");
}

//=========================================================//

void MagneticLine::calculateHeightScaleFactor(void)
{
	heightScaleFactor = cDistance(A, B)/values->stdBlockHeight;
}

//=========================================================//

void MagneticLine::setupInitialForceField(void)
{
	// Documentation: [Methods: 09 - Magnetic line's force field]

	double lineForce		= 0.3 * values->forceMax * values->forceScaleFactor;
	double sphereForce		= 0.4 * values->forceMax * values->forceScaleFactor;
	double lineDistance		= values->stdBlockRadius;
	double sphereDistance	= values->stdBlockHeight * heightScaleFactor;

	// horizontal magnetic force (depends on radius)
	lineShape->m_material.setMagnetMaxForce(lineForce);
	lineShape->m_material.setStiffness(0.4 * values->stiffnessMax);
	lineShape->m_material.setMagnetMaxDistance(lineDistance);
	magneticEffect = new cEffectMagnet(lineShape);
    lineShape->addEffect(magneticEffect);

	// vertical magnetic force (depends on height)
	sphereShape = new cShapeSphere(0.0001 * values->BLOCK_SCALE_FACTOR);	
	sphereShape->setPos(B);
	sphereShape->m_material.setMagnetMaxForce(sphereForce);
	sphereShape->m_material.setStiffness(0.4 * values->stiffnessMax);
	sphereShape->m_material.setMagnetMaxDistance(sphereDistance);
	magneticEffect = new cEffectMagnet(sphereShape);
	sphereShape->addEffect(magneticEffect);

	isForceFieldEnabled = false;
	block->getBottomMesh()->setAsGhost(true);
	block->getTopMesh()->setAsGhost(false);
	setForceFieldStatus(false);
}

//=========================================================//

void MagneticLine::scaleForce()
{
	double lineForce		= 0.3 * values->forceMax * values->forceScaleFactor;
	double sphereForce		= 0.4 * values->forceMax * values->forceScaleFactor;

	// horizontal magnetic force (depends on radius)
	lineShape->m_material.setMagnetMaxForce(lineForce);
	lineShape->m_material.setStiffness(0.4 * values->stiffnessMax);

	// vertical magnetic force (depends on height)
	sphereShape->m_material.setMagnetMaxForce(sphereForce);
}

//=========================================================//

void MagneticLine::scaleForce(double scaleFactor)
{
	double lineForce		= 0.3 * values->forceMax * values->forceScaleFactor * scaleFactor;
	double sphereForce		= 0.4 * values->forceMax * values->forceScaleFactor * scaleFactor;

	// horizontal magnetic force (depends on radius)
	lineShape->m_material.setMagnetMaxForce(lineForce);
	lineShape->m_material.setStiffness(0.4 * values->stiffnessMax);

	// vertical magnetic force (depends on height)
	sphereShape->m_material.setMagnetMaxForce(sphereForce);
}

//=========================================================//

void MagneticLine::updateHaptics(void)
{
	// Documentation: [Methods: 10 - Magnetic line activation/deactivation]
	if(values->tool->isInContact(block->getTopMesh()->getChild(0)))
	{
		if(!isForceFieldEnabled)
		{
			block->setHighlightBlockAsActive(true);
			isForceFieldEnabled = true;
			block->getTopMesh()->setAsGhost(true);
			block->getBottomMesh()->setAsGhost(false);
			setForceFieldStatus(true);
			if(this->isSecondLine)
			{
				// starts counting collisions and time once the second line is activated 
				values->outfileForces.open(("../Debug/OUTPUTFORCES.txt"));
				values->isInsideThePath = true;
				time(&values->startingTime);
				values->startingNumCollisions = values->numOfCollisions;
			}
		}
	}

	if(isForceFieldEnabled)
	{
		if(!this->isToolOriented)
		{
			// Documentation: [Methods: 02 - Auto-orienting the catheter tooltip's mesh]
			orientationCount++;	
			cQuaternion quat;
			cMatrix3d	rotMatrix;
			double directionAngle;

			cVector3d v1 = values->tool->getProxyGlobalPos() - values->toolTipEndSphere->getGlobalPos();
			v1.normalize();
			cVector3d v2 = this->getVector();
			v2.normalize();

			cVector3d axis = v1.crossAndReturn(v2);
			double angle = acos(v1.dot(v2));
			quat.fromAxisAngle(axis, angle);
			rotMatrix.identity();
			quat.toRotMat(rotMatrix);

			if(axis.equals(cVector3d(0,0,0))) // the case where the vector is parallel to the tool
			{
				directionAngle = acos((v1.dot(v2))/((v1.length())*(v2.length())));
				directionAngle = cRadToDeg(directionAngle);

				if(directionAngle !=0 )
				{
					values->tool->m_proxyMesh->rotate(cVector3d(1,0,0), 180);
					values->toolTipEndSphere->rotate(cVector3d(1,0,0), 180);
				}

			}else
			{
				values->tool->m_proxyMesh->rotate(rotMatrix);
				values->toolTipEndSphere->rotate(rotMatrix);
			}

		}

			if(orientationCount>50)
			this->isToolOriented = true;
	}

	if(values->tool->isInContact(block->getBottomMesh()->getChild(0)))
	{
		if(isForceFieldEnabled)
		{
			block->setHighlightBlockAsActive(false);

			isForceFieldEnabled = false;
			block->getBottomMesh()->setAsGhost(true);
			block->getTopMesh()->setAsGhost(false);
			setForceFieldStatus(false);
			if(this->isLastLine)
			{
				values->isInsideThePath = false;
				time(&values->endingTime);
				values->totalTime = difftime(values->endingTime, values->startingTime);
				values->endingNumCollisions = values->numOfCollisions;
				values->totalNumCollisions = values->endingNumCollisions - values->startingNumCollisions;
				if(values->numOfMidPoints>2)
				{
					// write calculated values of number of collisions and time taken to an external file
					values->outfileCollisionTime.open(("../Debug/OUTPUTCOLLISIONTIME.txt"));
					values->outfileCollisionTime<<values->totalTime<<" "<<values->totalNumCollisions<<endl;
					values->outfileCollisionTime.close();
				}
				values->outfileForces.close();
					
			}
		}
	}

	if(isForceFieldEnabled)
	{
		// Documentation: [Methods: 11 - Smoothing the magnetic effect between blocks]
		double distance = values->tool->getDeviceGlobalPos().distance(lineShape->m_pointA);
		double length = (A-B).length();
		double percentage = (distance / length) ;

		if(percentage  < 0.5)
		{
			scaleForce(percentage);
		}

	}else
	{
		this->isToolOriented = false;
		this->orientationCount = 0;
	}
	
	values->tool->applyForces();
}

//=========================================================//

/*=========================================================//
//==================[METHODS DEFINITIONS]==================//
//==================SETTERS AND GETTERS====================//
//=========================================================*/

void MagneticLine::setA(cVector3d A)
{
	this->A = A;
	vector = B - A;
}

//=========================================================//

void MagneticLine::setB(cVector3d B)
{
	this->B = B;
	vector = B - A;
}

//=========================================================//

void MagneticLine::setVector(cVector3d A, cVector3d B)
{
	this->A = A;
	this->B = B;
	vector = B - A;
	lineShape = new cShapeLine(A, B);
	lineShape->m_ColorPointA.set(1, 0, 0);
	lineShape->m_ColorPointB.set(1, 0, 0);
}

//=========================================================//

cVector3d MagneticLine::getVector()
{
	return vector;
}

//=========================================================//

cVector3d MagneticLine::getA()
{
	return A;
}

//=========================================================//

cVector3d MagneticLine::getB()
{
	return B;
}

//=========================================================//

cShapeLine* MagneticLine::getLineShape()
{
	return lineShape;
}

//=========================================================//

void MagneticLine::setBlock(VFBlock* block)
{
	this->block = block;	

	calculateHeightScaleFactor();
	setupInitialForceField();
}
//=========================================================//

void MagneticLine::setForceFieldStatus(bool status)
{
	if(status)	//if true
	{
		if(!isMagneticPathAdded)
		{
			values->world->addChild(lineShape);
			values->world->addChild(sphereShape);
			isMagneticPathAdded = true;
		}
	} else		//if false
	{
		if(isMagneticPathAdded)
		{
			values->world->removeChild(lineShape);
			values->world->removeChild(sphereShape);
			isMagneticPathAdded = false;
		}
	}
}

//=========================================================//

void MagneticLine::setGraphicalDisplayHidden(bool status)
{
	if(status)
	{
		lineShape->m_material.setTransparencyLevel(0);
		sphereShape->m_material.setTransparencyLevel(0);
	}
	else
	{
		lineShape->m_material.setTransparencyLevel(1);
		sphereShape->m_material.setTransparencyLevel(1);
	}
}

//=========================================================//

void MagneticLine::setLineTransparency(bool status)
{
	lineShape->setUseTransparency(true, true);
	if(status)
	{		
		lineShape->m_ColorPointA.setA(0);
		lineShape->m_ColorPointB.setA(0);
	}
	else
	{
		lineShape->m_ColorPointA.setA(1);
		lineShape->m_ColorPointB.setA(1);
	}
}

//=========================================================//