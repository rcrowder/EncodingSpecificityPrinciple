/* This is the module specification for the McGregor neural cell 
 * model module.
 */

mod_name		Neuron;
qualifier		McGregor;
proc_routine	RunModel;
typedef struct {

	Float	membraneTConst_Tm;	/* Membrane time constant (s). */	
	Float	kDecayTConst_TGk;	/* Potassium conductance decay constant (s). */
	Float	thresholdTConst_TTh;/* Threshold rise time constant (s). */
	Float	accomConst_c;		/* Accomdation constant (dimensionless). */
	Float	delayedRectKCond_b;	/* Delayed Rectifier K Cond. strength (ns). */
	Float	restingThreshold_Th0;/* Resting threshold of cell (mV). */
	Float	actionPotential;	/* Action potential of spike (mv). */
	Float	kEquilibriumPot_Ek;	/* Reversal pot. of the K conductance (mV). */
	Float	cellRestingPot_Er;	/* Resting potential for K conductance (mV). */

} McGregor;
