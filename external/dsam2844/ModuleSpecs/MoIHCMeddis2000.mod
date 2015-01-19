/* This is the module specification for the Meddis 2000 IHC
 * synapse module.
 */

mod_name		IHC;
qualifier		Meddis2000;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	opMode;	/* output probabiliy mode: spikes or probability */
	NameSpecifier   diagMode;	/* output variables during run (no/filename) */
	NameSpecifier	caCondMode;	/* Calcium conductance mode */
	NameSpecifier	cleftReplenishMode;	/* Cleft replenishment mode */
	long	ranSeed;		/* seed for random number generator */

	Float CaVrev;			/* Calcium reversal potential */
	Float betaCa; 			/* Boltzman function parameters */
	Float gammaCa;			/* Boltzman function parameters */ 

	Float pCa;  			/* Calcium current exponent */ 
	Float GCaMax;			/* Maximum calcium conductance */  
	Float perm_Ca0;  		/* Permeabilituy with zero Calcium concentration */
  	Float perm_z;			/* Permeability constant. */
        
	Float tauCaChan;		/* Time constant for the calcium channel*/        
	Float tauConcCa;		/* Time constant for the calcium concentration */

	int	maxFreePool_M;		/* Max. no. of transm. packets in free pool */
	Float	replenishRate_y;	/* Replenishment rate (units per second). */
	Float	lossRate_l;		/* Loss rate (units per second). */
	Float	reprocessRate_x;	/* Reprocessing rate (units per second). */
	Float	recoveryRate_r;		/* Recovery rate (units per second). */


} HairCell2;
