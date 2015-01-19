/* This is the module specification for the Laurel H. Carney IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Carney;
proc_routine	RunModel;

typedef struct {

	Float	maxHCVoltage;		/* Vmax */
	Float	restingReleaseRate;	/* R0 */
	Float	restingPerm;		/* Prest */
	Float	maxGlobalPerm;		/* PGmax */
	Float	maxLocalPerm;		/* PLmax */
	Float	maxImmediatePerm;	/* PImax */
	Float	maxLocalVolume;		/* VLmax */
	Float	minLocalVolume;		/* VLmin */
	Float	maxImmediateVolume;	/* VImax */
	Float	minImmediateVolume;	/* VImin */


} CarneyHC;
