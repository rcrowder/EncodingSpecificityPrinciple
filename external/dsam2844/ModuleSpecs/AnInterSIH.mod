/* This is the specification for the Inter-spike interval histogram
 * analysis module.
 */

mod_name		Analysis;
qualifier		ISIH;
proc_routine	Calc;
process_vars	true;
typedef struct {

	int		order;			/* How many spikes away, or -1 for all spikes */
	Float	eventThreshold;
	Float	maxInterval;	/* Max. interval or -1 for end of signal. */

} InterSIH;
