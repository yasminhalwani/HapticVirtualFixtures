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

typedef struct MagneticLine {

/*=========================================================//
//========================[PROTECTED]========================//
//=========================================================*/
protected:
	//========================[VARIABLES]======================//
	CommonValues*	values;
	// the starting point
	cVector3d		A;
	// the ending point
	cVector3d		B;
	// the vector representing this line
	cVector3d		vector;
	// the mesh associated with this line
	cShapeLine*		lineShape;
	// the mesh associated with the sphere at the end of the line
	cShapeSphere*	sphereShape;
	// the mesh representing the line with the sphere together
	cMesh*			magneticPath;
	// the block encompassing the line
	VFBlock*		block;
	// the magnetic effect applied to the line
	cEffectMagnet*	magneticEffect;
	// the scale factor for the line
	double			heightScaleFactor;
	// flags
	bool			isForceFieldEnabled;
	bool			isMagneticPathAdded;
	bool			isInsideBlock;
	bool			isToolOriented;
	int				orientationCount;

	//========================[METHODS]========================//
	// calculates the height scale factor w.r.t. the standard height
	// this helps to obtain the corresponding magnetic force proportional to the line's height
	void		calculateHeightScaleFactor(void);
	// scales the value of the force field according to the line's height
	void		scaleForce(double scaleFactor);

public:
	//========================[VARIABLES]======================//	
	MagneticLine*	next;
	bool			isSecondLine;
	bool			isLastLine;

	//========================[METHODS]========================//
	// constructors
	MagneticLine();
	MagneticLine(cVector3d A, cVector3d B);
	// sets up the initial force field of the line
	void		setupInitialForceField(void);
	// pass the block containing the line
	void		setBlock(VFBlock* block);
	// include this in the main haptic loop
	void		updateHaptics(void);
	// prints the details of the lines (its coordinates and length)
	void		print();
	// sets the forcefield of the line on or off
	void		setForceFieldStatus(bool status);
	// hides the graphical display of the lines
	void		setGraphicalDisplayHidden(bool status);
	// scales the central guidance magnetic force by the scale factor in CommonValues
	void		scaleForce();
	// sets the transparency of the red line representing the magnetic line
	void		setLineTransparency(bool status);

	//========================[METHODS]===setters & getters====//
	void		setA(cVector3d A);
	void		setB(cVector3d B);
	void		setVector(cVector3d A, cVector3d B);

	cVector3d	getA();
	cVector3d	getB();
	cVector3d	getVector();
	cShapeLine*	getLineShape();
	bool		getGuidance();
};