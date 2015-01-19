/* This is the module specification for the spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Simple;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	outputMode;
	long	ranSeed;
	int		numFibres;
	Float	pulseDurationCoeff;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	ParArrayPtr		distribution;

} SimpleSG;
