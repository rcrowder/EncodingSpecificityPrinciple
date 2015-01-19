/* This is the module specification for the Gammachirp module
 * membrane module.
 */

mod_name		BasilarM;
qualifier		GammaChirp;
proc_routine	RunModel;
process_vars	true;

typedef struct {

	NameSpecifier	diagnosticMode;	/* Mode for debug/diagnostics output */
	NameSpecifier	opMode; 			/* SwCntl */
	int	cascade;		/* n */
	Float	bCoeff;		/* b */
	Float	cCoeff0;	/* cCoef(0) */
	Float	cCoeff1;	/* cCoef(1) */
	Float	cLowerLim;	/* cLim(0) */
	Float	cUpperLim;	/* cLim(1) */

	CFListPtr	theCFs;	/* Pointer to centre frequency structure. */

} BMGammaC;
