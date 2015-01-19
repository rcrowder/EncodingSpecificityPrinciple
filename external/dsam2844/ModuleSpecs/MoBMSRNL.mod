/* This is the module specification for the SRNL basilar
 * membrane module.
 * This BM model
 */

mod_name		BasilarM;
qualifier		SRNL;
proc_routine	RunModel;

typedef struct {

	int				nonLinGTCascade;
	int				nonLinLPCascade;
	ParArrayPtr		nonLinBwidth;
	ParArrayPtr		nonLinScaleG;

	ParArrayPtr		comprScaleA;
	ParArrayPtr		comprScaleB;
	Float			comprExponent;
	
	int				linGTCascade;
	int				linLPCascade;
	ParArrayPtr		linCF;
	ParArrayPtr		linBwidth;
	ParArrayPtr		linScaleG;

	NameSpecifier	stapesMode;
	Float			stapesCF;
	CFListPtr		theCFs;

} BMSRNL;

