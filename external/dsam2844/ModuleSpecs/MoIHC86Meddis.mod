/* This is the module specification for the Meddis 86 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis86;
proc_routine	RunModel;

typedef struct {

	NameSpecifier   diagMode;	/* output variables during run (no/filename) */
	Float	permConst_A;		/* Permeability constant (?). */
	Float	permConst_B;		/* Units per second */
	Float	releaseRate_g;		/* Release rate (units per second). */
	Float	replenishRate_y;	/* Replenishment rate (units per second). */
	Float	lossRate_l;			/* Loss rate (units per second). */
	Float	reprocessRate_x;	/* Reprocessing rate (units per second). */
	Float	recoveryRate_r;		/* Recovery rate (units per second). */
	Float	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	Float	firingRate_h;		/* Firing rate (spikes per second). */

} HairCell;
