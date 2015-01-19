/* This is the module specification for the Laurel H. Carney IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Cooke91;
proc_routine	RunModel;

typedef struct {

	Float	crawfordConst;		/* Crawford and Fettiplace c Value */
	Float	releaseFraction;	/* Release fraction. */
	Float	refillFraction;		/* Replenishment fraction. */
	Float	spontRate;			/* desired spontaneous firing rate. */
	Float	maxSpikeRate;		/* maximum possible firing rate. */

} CookeHC;
