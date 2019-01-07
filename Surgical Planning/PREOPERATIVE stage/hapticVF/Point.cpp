/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Surgical Planning - Preoperative Stage

[Point]
Serves as a structure for the midpoint in the software; storing the point's 
location and graphical representation.
Built for linked lists.

For more details, please refer to the documentation.

Developed by Yasmin Halwani		(yasmin.halwani@outlook.com)
Supervised by Dr. Osama Halabi	(ohalabi@qu.edu.qa)
Computer Science and Engineering Department
Qatar University
2014
****************************************************************************/

#include "stdafx.h"

Point:: Point(cVector3d point)
{
	values = CommonValues::getInstance();

	this->point = point;

	// sets the initial radius of the point as 0.03
	pointDraw = new cShapeSphere(0.03);
	values->world->addChild(pointDraw);
	pointDraw->setPos(point);
	
}

//---------------------------------------------------------------------------

Point::Point()
{
	values = CommonValues::getInstance();

	this->point = cVector3d(0,0,0);

	pointDraw = new cShapeSphere(0.03);
	values->world->addChild(pointDraw);
}

//---------------------------------------------------------------------------