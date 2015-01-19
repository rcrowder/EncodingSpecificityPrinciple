/* This is the module specification for the AnAverage module
 * This process calculates the average value for each channel.
 * It incorporates the half-wave rectified averages originally written
 * by Trevor Shackleton.
 */

mod_name		Analysis;
qualifier		Averages;
proc_routine	Calc;
typedef struct {

	NameSpecifier	mode;	/* full, -wave, +wave */
	Float	timeOffset;
	Float	timeRange;

} Averages;
