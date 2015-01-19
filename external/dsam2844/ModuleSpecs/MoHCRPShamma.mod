/* This is the module specification for the Shamma IHC receptor
 * potential module.
 */

mod_name		IHCRP;
qualifier		Shamma;
proc_routine	RunModel;

typedef struct {

	Float	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	Float	reversalPot_Ek;		/* Reversal potential, mV. */
	Float	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	Float	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	Float	restingConductance_G0;	/* Resting Conductance, G0. */
	Float	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	Float	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	Float	beta;				/* beta = exp(-G1/RT), dimensionless. */
	Float	gamma;				/* gamma = Z1/RT, u/m. */
	Float	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	Float	ciliaCouplingGain_C;/* Cilia coupling gain. */
	Float	referencePot;		/* Reference potential */

} Shamma;
