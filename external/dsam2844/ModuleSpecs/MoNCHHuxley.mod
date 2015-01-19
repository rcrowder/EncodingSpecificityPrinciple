/* This is the module specification for the Ray Meddis' implementation
 * of the Hudgkin Huxley module.
 * The parameters appear in the order that the equation was originally written.
 */

mod_name		Neuron;
qualifier		HHuxley;
proc_routine	RunModel;
typedef		int	IonChanListPtr;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	operationMode;
	NameSpecifier	injectionMode;
	Float			excitatoryReversalPot;
	Float			inhibitoryReversalPot;
	Float			shuntInhibitoryReversalPot;
	Float			cellCapacitance;
	Float			restingPotential;
	Float			restingSignalDuration;
	Float			restingCriteria;
	IonChanListPtr	iCList;
	
	/* Private members */

} HHuxleyNC;
