/* This is the module specification for the Zhang et al. basilar
 * membrane module.
 */

mod_name		BasilarM;
qualifier		Zhang;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	model;		/* Model to use. */
	NameSpecifier	species;	/* Species to use. */
	NameSpecifier	microPaInput;	/* Expect input in micro Pascals. */
	int		nbOrder;			/* Order of the narrow bandpass filter */
	int		wbOrder;			/* Order of the wide bandpass filter */
	Float	cornerCP;			/* Parameter in logarithmic nonlinearity */
	Float	slopeCP;			/* Parameter in logarithmic nonlinearity */
	Float	strenghCP;			/* Parameter in logarithmic nonlinearity */
	Float	x0CP;				/* parameter in Boltzman function */
	Float	s0CP;				/* parameter in Boltzman function */
	Float	x1CP;				/* parameter in Boltzman function */
	Float	s1CP;				/* parameter in Boltzman function */
	Float	shiftCP;			/* parameter in Boltzman function */
	Float	cutCP;	/* Cut-off frequency of control-path low-path filter (Hz) */
	int		kCP;	/* Order of control-path low-pass filter */
	Float	r0;		/* Ratio of tau_LB (lower bound of tau_SP) to tau_narrow */
	CFListPtr	cFList;			/* Pointer to centre frequency structure. */

} BMZhang;
