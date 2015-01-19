/* This is the module specification for the CMR with stochastic masker module
 */

mod_name		CMR;
qualifier		NoiseMasker;
proc_routine	GenerateSignal;
process_vars	true;
typedef struct {

	BoolSpecifier	binauralMode;
	NameSpecifier	spacingType;
	NameSpecifier	condition;
	NameSpecifier	onFreqEar;
	NameSpecifier	sigEar;
	char	flankEar;
	int		nlow;		/* GetInt("Lower bands"); */
	int		nupp;		/* GetInt("Upper bands"); */
	int		nGapLow;
	int		nGapUpp;
	long	ranSeed;
	Float	lowFBLevel;
	Float	uppFBLevel;
	Float	oFMLevel;		/* GetDoubleAs("OFM level","ratio") */
	Float	spacing;
	NameSpecifier	bandwidthMode;
	Float	bandwidth;
	Float	mskModFreq;
	Float	mskModPhase;
	Float	sigLevel;
	Float	sigFreq;
	NameSpecifier	sigGateMode;
	Float	gateTime;
	Float	duration, dt;

} CMRNoiseM;
