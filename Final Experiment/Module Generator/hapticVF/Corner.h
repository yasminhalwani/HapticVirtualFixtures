/***************************************************************************
Graphical/Haptic Simulation for Heart Catheterization Telerobotic Surgery
Software: Final Experiment - Module Generator

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

typedef struct Corner: VFBlock{

/*=========================================================//
//========================[PROTECTED]======================//
//=========================================================*/
protected:	
	//========================[VARIABLES]======================//

	//========================[METHODS]========================//
	// loads the meshes specific to the corner shape
	void		redefineMeshes(void);
	// adjusts the size of the corner to match the cylinder's proportions
	void		modifySize(void);


/*=========================================================//
//========================[PUBLIC]=========================//
//=========================================================*/
public:
	//========================[VARIABLES]======================//
	Corner* next;

	//========================[METHODS]========================//
	Corner();
	void		setVertexPos(int faceNum, int vertexNum, cVector3d pos);
	cVector3d	getVertexPos(int faceNum, int vertexNum);
	void		translateVertex(int faceNum, int vertexNum, cVector3d tran);
	void		recalculateCollision();
	void		rotateBottom(cVector3d rotAxis, double rotAngleDegrees);


/*=========================================================//
//========================[DECLARATIONS]===================//
//=========================================================*/

};