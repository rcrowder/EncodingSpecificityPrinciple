/* This is a module specifiction file for the DataFile module */
mod_name		DataFile;
proc_routine	ReadSignal;
typedef struct {

	char	*name;			/* [MAX_FILE_PATH] */
	NameSpecifier	subFormatType;
	int		wordSize;
	int		endian;
	int		numChannels;
	Float	defaultSampleRate;
	Float 	duration;
	Float	timeOffset;
	Float	gain;
	Float	normalisation;

} DataFile;
	
