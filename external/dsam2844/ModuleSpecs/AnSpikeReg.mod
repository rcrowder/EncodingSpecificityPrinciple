/* This is the specification for the Spike interval regularity analysis module.
 */

mod_name		Analysis;
qualifier		SpikeRegularity;
proc_routine	Calc;
typedef struct {

	NameSpecifier	outputMode;
	Float	eventThreshold;
	Float	windowWidth;
	Float	timeOffset;
	Float	timeRange;
	Float	deadTime;
	Float	countThreshold;

} SpikeReg;
