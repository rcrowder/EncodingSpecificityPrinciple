/* This is the module specification for the Average Channels utility
 * module.
 * The mode can be set to "Average" or simple "sum" of channels.
 */

mod_name		Utility;
qualifier		ReduceChannels;
typedef struct {

	NameSpecifier	mode;
	int		numChannels;

} ReduceChans;
