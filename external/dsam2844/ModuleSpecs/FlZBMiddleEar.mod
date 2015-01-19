/* This is the module specification for the Zilany and Bruce middle-ear
 * filter module.
 * Zilany, M. S. A. and Bruce, I. C. (2006). "Modeling auditory-nerve responses
 * for high sound pressure levels in the normal and impaired auditory periphery,"
 * Journal of the Acoustical Society of America 120(3):1446-1466.
 * Zilany, M. S. A. and Bruce, I. C. (2007). "Representation of the vowel /eh/
 * in normal and impaired auditory nerve fibers: Model predictions of responses
 * in cats," Journal of the Acoustical Society of America 122(1):402-417.
 */

mod_name		Filter;
qualifier		ZBMiddleEar;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	Float	gainMax;			/* Middle ear maximum gain. */
	Float	preWarpingFreq;		/* prewarping frequency 1 kHz */

} FLZBMEar;

