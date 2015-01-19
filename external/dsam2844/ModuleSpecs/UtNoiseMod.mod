/* This is the module specification for the Noise Amplitude Modulation Utility module
 */

mod_name		Utility;
qualifier		AmpMod_Noise;
process_vars	true;
typedef struct {

	NameSpecifier	bandwidthMode;
	Float	bandwidth;
	long	ranSeed;

} NAmpMod;
	
