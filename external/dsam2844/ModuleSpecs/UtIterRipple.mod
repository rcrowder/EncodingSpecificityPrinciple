/* This is the module specification for the StIRN module
 */

mod_name		Utility;
qualifier		IteratedRipple;
typedef struct {

	int		numIterations;
	NameSpecifier	mode;
	Float	delay;
	Float	signalMultiplier;

} IterRipple;
