/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Initial Experiment 

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

typedef struct Point{

public:

	Point* next;
	cVector3d point;	
	Point(cVector3d point);
	Point();
	
};