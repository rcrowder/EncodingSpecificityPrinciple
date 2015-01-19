/* This is the module specification for the multi-fibre spike generation
 * module.
 */

mod_name		ANSpikeGen;
qualifier		Binomial;
proc_routine	RunModel;
process_vars	true;
typedef struct {

	NameSpecifier	diagnosticMode;
	NameSpecifier	outputMode;
	int		numFibres;
	long	ranSeed;
	Float	pulseDurationCoeff;
	Float	pulseMagnitude;
	Float	refractoryPeriod;
	ParArrayPtr		distribution;

} BinomialSG;
