/* This is the module specification for the UtStrobe.c module
 * It generates strobe points to be used by the AnSAImage module.
 */

mod_name		Utility;
qualifier		Strobe;
typedef struct {

	int		typeMode;
	int		diagnosticMode;
	Float	threshold;
	Float	thresholdDecayRate;
	Float	delay;
	Float	delayTimeout;

} Strobe;

