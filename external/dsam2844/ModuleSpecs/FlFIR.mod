/* This is the module specification for the FIR filter module.
 */

mod_name		Filter;
qualifier		FIR;
proc_routine	RunProcess;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	type;
	int		numTaps;
	int_al	numBands;
	Float	*bandFreqs, *desired, *weights;

}  FIR;
