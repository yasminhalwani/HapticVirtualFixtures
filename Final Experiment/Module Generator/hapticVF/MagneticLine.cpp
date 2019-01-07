/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Final Experiment - Module Generator

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

	if(!values->V)
		setLineAsTransparent(true);
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
	double lineForce		= 0.3 * values->forceMax * values->forceScaleFactor;
	double sphereForce		= 0.4 * values->forceMax * values->forceScaleFactor;
	double lineDistance		= values->stdBlockRadius;
	double sphereDistance	= values->stdBlockHeight * heightScaleFactor;

	// horizontal magnetic force (depends on radius)
	lineShape->m_material.setMagnetMaxForce(lineForce);
	lineShape->m_material.setStiffness(0.4 * values->stiffnessMax);
	lineShape->m_material.setMagnetMaxDistance(lineDistance);
	magneticEffect = new cEffectMagnet(lineShape);
    if(values->G)lineShape->addEffect(magneticEffect);

	// vertical magnetic force (depends on height)
	sphereShape = new cShapeSphere(0.0001 * values->BLOCK_SCALE_FACTOR);	
	sphereShape->setPos(B);
	sphereShape->m_material.setMagnetMaxForce(sphereForce);
	sphereShape->m_material.setStiffness(0.4 * values->stiffnessMax);
	sphereShape->m_material.setMagnetMaxDistance(sphereDistance);
	magneticEffect = new cEffectMagnet(sphereShape);
	if(values->G)sphereShape->addEffect(magneticEffect);
	
	setGuidance(true);
	isForceFieldEnabled = false;
	block->getBottomMesh()->setAsGhost(true);
	block->getTopMesh()->setAsGhost(false);
	setForceFieldStatus(false);
}

//=========================================================//

void MagneticLine::updateHaptics(void)
{
	if(isGuidanceOn)
	{
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
					stringstream ss;
					ss << values->trials;
					string trialNum = ss.str();
					string extension = ".txt";
					string fileName = "../Debug/OUTPUT_FORCES_" + values->moduleName + string("_trial") + trialNum + extension;
					values->outfileForces.open((fileName));
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
					if(values->trials==1)values->totalTime1 = difftime(values->endingTime, values->startingTime);
					if(values->trials==2)values->totalTime2 = difftime(values->endingTime, values->startingTime);
					if(values->trials==3)values->totalTime3 = difftime(values->endingTime, values->startingTime);
					values->endingNumCollisions = values->numOfCollisions;
					if(values->trials==1)values->totalNumCollisions1 = values->endingNumCollisions - values->startingNumCollisions;
					if(values->trials==2)values->totalNumCollisions2 = values->endingNumCollisions - values->startingNumCollisions;
					if(values->trials==3)values->totalNumCollisions3 = values->endingNumCollisions - values->startingNumCollisions;
					if(values->numOfMidPoints>2)
					{
						if(!values->isTutorialModule)values->trials++;
						if(values->trials>=4)
						{
							cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
							cout<<"Thank you for your patience."<<endl;
							cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
							cout<<"Please rate the effectivenes of guidance of this module."<<endl;
							cout<<"Effectivenss of guidance: the easieness of navigating from"<<endl;
							cout<<"the beginning till the end with the highest accuracy in the"<<endl;
							cout<<"shortest time period."<<endl;
							cout<<"Please enter the number corresponding to your rating:"<<endl;
							cout<<"[5] - Excellent"<<endl;
							cout<<"[4] - Very Good"<<endl;
							cout<<"[3] - Good"<<endl;
							cout<<"[2] - Not Bad"<<endl;
							cout<<"[1] - Poor"<<endl;
							cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

							int rating;
							cin>>rating;
							cout<<endl;
							cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;

							values->averageTime = (values->totalTime1 + values->totalTime2 + values->totalTime3)/3;
							values->averageTotalNumCollisions = (values->totalNumCollisions1 +
								values->totalNumCollisions2 + values->totalNumCollisions3)/3;

							string fileName;
							stringstream ss;
							ss << values->trials - 1;
							string trialNum = ss.str();
							string extension = ".txt";

							fileName = "../Debug/OUTPUT_VALUES_" + values->moduleName + extension;
							
							values->outfileCollisionTime.open((fileName));
							values->outfileCollisionTime<<"Total Time1: "<<values->totalTime1<<" , ";
							values->outfileCollisionTime<<"Collisions1: "<<values->totalNumCollisions1<<endl;
							values->outfileCollisionTime<<"Total Time2: "<<values->totalTime2<<" , ";
							values->outfileCollisionTime<<"Collisions2: "<<values->totalNumCollisions2<<endl;
							values->outfileCollisionTime<<"Total Time3: "<<values->totalTime3<<" , ";
							values->outfileCollisionTime<<"Collisions3: "<<values->totalNumCollisions3<<endl;
							values->outfileCollisionTime<<"Average Total Time: "<<values->averageTime<<" , ";
							values->outfileCollisionTime<<"Average Total Num of Collisions: "<<values->averageTotalNumCollisions<<endl;
							values->outfileCollisionTime<<"Rating: "<<rating<<endl;
							values->outfileCollisionTime.close();

							cout<<"Module [" << values->moduleName << "] has ended.\n";
							cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
							exit(0);
							values->hasModuleEnded = true;
						}else
							if(!values->isTutorialModule)printf("Trial # %d\nPlease change the orientation to perform the next trial.\nTo change the orientation, press the haptic switch and move the camera around.\n", values->trials);
					}
					values->outfileForces.close();
					
				}
			}
		}

		if(isForceFieldEnabled)
		{
			double distance = values->tool->getDeviceGlobalPos().distance(lineShape->m_pointA);
			double length = (A-B).length();
			double percentage = (distance / length) ;

			if(percentage  < 0.2)
			{
				isThresholdPassed = false;
				scaleForce(percentage);
			}
			else if(!isThresholdPassed)
			{
				if(!block->isCorner)
				scaleForce(1.2);
				isThresholdPassed = true;
			}

		}else
		{
			this->isToolOriented = false;
			this->orientationCount = 0;
		}
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
	setGuidance(true);
}

//=========================================================//

void MagneticLine::setGuidance(bool status)
{
	this->isGuidanceOn = status;
}

//=========================================================//

bool MagneticLine::getGuidance()
{
	return this->isGuidanceOn;
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

void MagneticLine::setLineAsTransparent(bool status)
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
