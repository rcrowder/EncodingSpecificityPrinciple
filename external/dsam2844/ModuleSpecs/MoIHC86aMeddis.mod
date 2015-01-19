/* This is the module specification for the Meddis 86 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis86a;
proc_routine	RunModel;

typedef struct {

	Float	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	Float	replenishRate_y;	/* Replenishment rate (units per second). */
	Float	lossRate_l;			/* Loss rate (units per second). */
	Float	reprocessRate_x;	/* Reprocessing rate (units per second). */
	Float	recoveryRate_r;		/* Recovery rate (units per second). */
	Float	permeabilityPH_h;	/* Permeability constant. */
	Float	permeabilityPZ_z;	/* Permeability constant. */
	Float	firingRate_h2;		/* Permeability constant. */

} HairCell3;
