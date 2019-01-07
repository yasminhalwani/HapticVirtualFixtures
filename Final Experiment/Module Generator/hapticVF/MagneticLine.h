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

typedef struct MagneticLine {

/*=========================================================//
//========================[PROTECTED]========================//
//=========================================================*/
protected:
	//========================[VARIABLES]======================//
	cVector3d		A;
	cVector3d		B;
	cVector3d		vector;
	cShapeLine*		lineShape;
	CommonValues*	values;
	cShapeSphere*	sphereShape;
	cMesh*			magneticPath;
	VFBlock*		block;
	cEffectMagnet*	magneticEffect;
	double			heightScaleFactor;
	bool			isGuidanceOn;
	bool			isForceFieldEnabled;
	bool			isMagneticPathAdded;
	bool			isInsideBlock;

	bool			isToolOriented;
	int				orientationCount;
	bool			isThresholdPassed;

	//========================[METHODS]========================//
	// calculates the height scale factor w.r.t. the standard height
	// this helps to obtain the corresponding magnetic force proportional to the line's height
	void		calculateHeightScaleFactor(void);
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
	// sets the red line representing the magnetic line transparent
	void		setLineAsTransparent(bool status);

	//========================[METHODS]===setters & getters====//
	void		setA(cVector3d A);
	void		setB(cVector3d B);
	void		setVector(cVector3d A, cVector3d B);
	void		setGuidance(bool status);

	cVector3d	getA();
	cVector3d	getB();
	cVector3d	getVector();
	cShapeLine*	getLineShape();
	bool		getGuidance();
};