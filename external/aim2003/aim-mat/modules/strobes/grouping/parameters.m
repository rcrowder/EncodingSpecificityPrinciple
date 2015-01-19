% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/15 09:48:33 $
% $Revision: 1.2 $
%%%%%%%%%%%%%
% strobes
% hidden parameters
grouping.generatingfunction='gen_grouping';
grouping.displayname='strobe finding grouping';
grouping.revision='$Revision: 1.2 $';

% parameters relevant for the calculation of this module
grouping.integrage_octaves_below=0.5;  % in octaves
grouping.integrage_octaves_above=0.5;  % in octaves
% delta_gamma.frequency_integration_=1;  % in octaves

grouping.short_time_constant=0.003;  % in seconds
grouping.window_function='linear'; % can be 'linear' or 'hamming'
% channel_integration.long_time_constant=0.03;  % in seconds

% channel_integration.sigmoid_slope=2;
% channel_integration.threshold_decay_constant=0.2;   % per ms

