/* This is the module specification for the Amplitude Modulation Utility module
 */

mod_name		Utility;
qualifier		AmpMod;
typedef struct {

	int_al	numFrequencies;
	Float	*modulationDepths, *frequencies, *phases;

} AmpMod;
	
