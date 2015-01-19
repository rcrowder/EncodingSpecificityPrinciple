/* This is the module specification for the harmonic stimulus
 * generation module.
 *
 */

mod_name		Harmonic;
proc_routine	GenerateSignal;
process_vars	true;
typedef struct {

	int		lowestHarmonic;
	int		highestHarmonic;
	int		mistunedHarmonic;
	int		order;
	NameSpecifier		phaseMode;
	Float	phaseVariable;
	Float	mistuningFactor;
	Float	frequency;
	Float	intensity;
	Float	duration, dt;
	Float	modulationFrequency;
	Float	modulationPhase;
	Float	modulationDepth;
	Float	lowerCutOffFreq;
	Float	upperCutOffFreq;

} Harmonic;
