/* This is the module specification for the AnPeriodHist module
 * aggregate mode:-
 *		0:			reset to zero on successive runs.
 *		nonzero:	collect responses from successive runs;
 * collection mode:
 *		0: 			spike detection;
 *		nonzero:	continuous signal (spike probabilities?);
 * type mode:
 *		0: 			Post stimulus time histogram (PSTH);
 *		nonzero:	Period histogram (PH);
 */

mod_name		Analysis;
qualifier		Histogram;
proc_routine	Calc;
typedef struct {

	NameSpecifier	detectionMode;
	NameSpecifier	typeMode;
	Float	eventThreshold;
	Float	binWidth;
	Float	period;
	Float	timeOffset;

} Histogram;
