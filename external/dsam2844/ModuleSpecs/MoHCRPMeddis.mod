/* This is the module specification for the Meddis IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Meddis;
proc_routine	RunModel;

typedef struct {

	Float	permConst_A;		/* For K+ probability: base Amplitude (uPa). */
	Float	permConst_B;		/* For K+ probability: amplitude scale (/uPa) */
	Float	releaseRate_g;		/* Release rate (units per second). */
	Float	mTimeConst_tm;		/* Membrane time constant, s. */

} MeddisRP;
