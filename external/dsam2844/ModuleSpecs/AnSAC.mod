/* This is the specification for the shuffled autocorrelogram
 * analysis module.
 */


mod_name		Analysis;
qualifier		SAC;
proc_routine	Calc;
process_vars	true;
typedef struct {

	BOOLN	normalisation;	/* Turn normalisation on and off */
	int		order;			/* How many spikes away, or -1 for all spikes */
	Float	eventThreshold;
	Float	maxInterval;	/* Max. interval or -1 for end of signal. */
	Float	binWidth;

} SAC;
