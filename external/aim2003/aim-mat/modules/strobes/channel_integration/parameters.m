% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/19 09:32:22 $
% $Revision: 1.3 $
%%%%%%%%%%%%%
% strobes
% hidden parameters
channel_integration.generatingfunction='gen_grouping';
channel_integration.displayname='strobe finding grouping';
channel_integration.revision='$Revision: 1.3 $';

% parameters relevant for the calculation of this module
channel_integration.integrage_octaves_below=0.5;  % in octaves
channel_integration.integrage_octaves_above=0.5;  % in octaves
% delta_gamma.frequency_integration_=1;  % in octaves

channel_integration.short_time_constant=0.003;  % in seconds
channel_integration.window_function='linear'; % can be 'linear' or 'hamming'
% channel_integration.long_time_constant=0.03;  % in seconds

% channel_integration.sigmoid_slope=2;
% channel_integration.threshold_decay_constant=0.2;   % per ms

