mod_name IHCRP;
qualifier Shamma3StateVelIn;
proc_routine RunModel;
process_vars true;

typedef struct {

	Float	endocochlearPot_Et;	/* Endocochlear potential, mV. */
	Float	reversalPot_Ek;		/* Reversal potential, mV. */
	Float	reversalPotCorrection;	/* Rp/(Rt+Rp), mV. */
	Float	totalCapacitance_C;	/* Total capacitance C = Ca + Cb, pF. */
	Float	restingConductance_G0;	/* Resting Conductance, G0. */
	Float	kConductance_Gk;	/* Potassium conductance, S (Seimens). */
	Float	maxMConductance_Gmax;	/* Maximum mechanical conductance, S. */
	Float	ciliaTimeConst_tc;	/* BM/cilia displacement time constant, s. */
	Float	ciliaCouplingGain_C;	/* Cilia coupling gain. */
	Float	referencePot;		/* Reference potential */
	Float  sensitivity_s0;		 /* Sensitivity constants for trans. function */
	Float  sensitivity_s1;
	Float  offset_u0;			  /* Offset constants for transduction function */
	Float  offset_u1;			  
  	
} Sham3StVIn;

