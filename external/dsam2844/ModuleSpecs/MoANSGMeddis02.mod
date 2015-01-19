/* This is the module specification for the Meddis 2002 spike generation module
 */

mod_name		ANSpikeGen;
qualifier		Meddis02;
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
	Float	recoveryTau;
	ParArrayPtr		distribution;

} Meddis02SG;
