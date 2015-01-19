/* This is the module specification for the Laurel H. Carney IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Carney;
proc_routine	RunModel;

typedef struct {

	Float		cutOffFrequency;	/* Fc */
	Float		hCOperatingPoint;	/* PDihc */
	Float		asymmetricalBias;	/* P0 */
	Float		maxHCVoltage;		/* Vmax */
	Float		waveDelayCoeff;		/* AD */
	Float		waveDelayLength;	/* AL */
	Float		referencePot;		/* Added for compatibility. */

} CarneyRP;
