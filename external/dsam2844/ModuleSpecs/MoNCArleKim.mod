/* This is the module specification for the Arle/Kim version of the
 * McGregor model module.
 */

mod_name		Neuron;
qualifier		ArleKim;
proc_routine	RunModel;
typedef struct {

	Float	membraneTConst_Tm;	/* Membrane time constant (s). */	
	Float	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	Float	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	Float	accomConst_c;		/* Accomdation constant (dimensionless). */
	Float	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	Float	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	Float	actionPotential;	/* Action potential of spike (mv). */
	Float	nonLinearVConst_Vnl;/* Non-linear voltage constant (mV). */
	Float	kReversalPoten_Ek;	/* Reversal pot. of the K conductance (mV). */
	Float	bReversalPoten_Eb;	/* Rev. pot. of all other conductances (mV) */
	Float	kRestingCond_gk;	/* Resting potassium conductance (nS). */
	Float	bRestingCond_gb;	/* Resting component for all other conds.(nS).*/	
	

} ArleKim;
