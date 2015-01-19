/* This is the module specification for the average localised synchronised
 * rate.
 * It expects multi-channel input from a period histogram
 * Yound E. D. and Sachs M. B. (1979) "Representation of steady-state vowels
 * in the temporal aspects of the discharge patterns of populations of auditory-
 * nerve fibers", J. Acoust. Soc. Am, Vol 66, pages 1381-1403.
 */

mod_name		Analysis;
qualifier		ALSR;
proc_routine	Calc;
process_vars	true;
typedef struct {

	Float	lowerAveLimit;
	Float	upperAveLimit;
	Float	normalise;

} ALSR;
