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

typedef struct VFBlock{

/*=========================================================//
//========================[PROTECTED]======================//
//=========================================================*/
protected:
	
	//========================[VARIABLES]======================//

	CommonValues*			values;

	cMesh*					cylinder;
	cMesh*					top; 
	cMesh*					bottom;	
	
	cShapeSphere*			topSideSphere;
	cShapeSphere*			bottomCenterSphere;
	cShapeSphere*			topCenterSphere;
	cShapeSphere*			bottomSideSphere;
	
	double					height;
	double					radius;

	bool					isGhost;
	bool					isHidden;
	bool					isStiffnessEnabled;

	bool					collisionFlag;

	cMaterial				cylinderMaterial;
	

	//========================[METHODS]========================//

	// loads the meshes for the cylinder, top and bottom
	void			importMeshes(void);
	// sets up the initial properties of the meshes
	void			setupInitialMeshesProperties(void);
	// measures the initial dimensions of the cylinder: radius and height
	void			measureInitialCylinderDimensions(void);

/*=========================================================//
//========================[PUBLIC]=========================//
//=========================================================*/
public:
	
	//========================[VARIABLES]======================//

	VFBlock*				next; 
	bool					isCorner;

	//========================[METHODS]========================//

	// constructor
	VFBlock();
	// called inside the main haptic loop in the program
	void		updateHaptics(void);
	// sets the VFBlock as a ghost; no collision enabled
	void		setAsGhost(bool status);	
	// if set to true, the VFBlock is no longer graphically visible
	void		setHide(bool status);
	// disables stiffness of the block if false
	void		setStiffnessStatus(bool status);
	// removes the block from the world
	void		removeFromWorld(void);
	// writes force data to file (adds a new row)
	void		writeForceToFile(cVector3d force);
	// highlights the color of the VFBlock to reddish
	void		setHighlightBlockAsActive(bool status);

	//========================[METHODS]=====transformations===//

	// performs unifrom scaling of the VFBlock
	void		scale(double scaleFactor);
	// scales the height of the VFBlock only
	void		scaleHeight(double scaleFactor);
	// scales the radius of the VFBlock only
	void		scaleRadius(double scaleFactor);
	// performs translation for the VFBlock
	void		translate(double x, double y, double z);
	// performs translation for the VFBlock
	void		translate(cVector3d translationVector);
	// rotates the VFBlock around a given axis and an angle in degrees
	void		rotate(cVector3d axisVector, double rotationAngleDegrees);
	// rotates the VFBlock with a given rotation matrix
	void		rotate(cMatrix3d rotationMatrix);
	// sets the position of the VFBlock
	void		setPos(double x, double y, double z);
	// sets the position of the VFBlock
	void		setPos(cVector3d locationVector);

	//========================[METHODS]=====setters & getters=//

	// sets the default transparency level of the VFBlock
	void		setDefaultTransparencyLevel(double transparencyLevel);
	// returns the default transparency level of the VFBlock
	double		getDefaultTransparencyLevel();
	// returns the current height of the VFBlock
	double		getHeight();
	// returns the current radius of the VFBlock
	double		getRadius();
	// returns the current global position of the center of the top of the block
	cVector3d	getTopCenterGlobalPos();
	// returns the current global posision of the side of the top of the block
	cVector3d	getTopSideGlobalPos();
	// returns the current global position of the center of the bottom of the block
	cVector3d	getBottomCenterGlobalPos();
	// returns the current global position of the side of the bottom of the block
	cVector3d	getBottomSideGlobalPos();	
	// returns the current position of the VFBlock
	cVector3d	getPos();
	// retrurns the rotation matrix of the VFBlock
	cMatrix3d	getRot();
	// returns the cMesh* object of the cylinder body
	cMesh*		getCylinderMesh();
	// returns the cMesh* object of the cylinder top
	cMesh*		getTopMesh();
	// returns the cMesh* object of the cylinder bottom
	cMesh*		getBottomMesh();



/*=========================================================//
//========================[DECLARATIONS]===================//
//=========================================================*/
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

};