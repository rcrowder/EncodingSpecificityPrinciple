/* This is the module specification for the CMR with deterministic
 * harmonic masker module
 */

mod_name		CMR;
qualifier		HarmonicMasker;
proc_routine	GenerateSignal;
typedef struct {

	BoolSpecifier	binauralMode;
	NameSpecifier	spacingType;
	NameSpecifier	condition;
	NameSpecifier	onFreqEar;
	NameSpecifier	sigEar;
	NameSpecifier	sigMode;
	char	flankEar;
	int		nlow;		/* GetInt("Lower bands"); */
	int		nupp;		/* GetInt("Upper bands"); */
	int		nGapLow;
	int		nGapUpp;
	Float	lowFBLevel;
	Float	uppFBLevel;
	Float	oFMLevel;
	Float	spacing;
	Float	mskmodfreq;
	Float	sigLevel;
	Float	sigFreq;
	Float	gateTime;
	Float	duration, dt;

} CMRHarmM;

