% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/09 15:24:56 $
% $Revision: 1.1 $
%%%%%%%%%%%%%
% strobes
% hidden parameters
delta_gamma.generatingfunction='gen_delta_gamma';
delta_gamma.displayname='strobe finding delta_gamma';
delta_gamma.revision='$Revision: 1.1 $';

% parameters relevant for the calculation of this module
delta_gamma.integrage_octaves_below=1;  % in octaves
delta_gamma.integrage_octaves_above=1;  % in octaves
% delta_gamma.frequency_integration_=1;  % in octaves

delta_gamma.short_time_constant=0.003;  % in seconds
delta_gamma.long_time_constant=0.03;  % in seconds

delta_gamma.sigmoid_slope=2;
delta_gamma.threshold_decay_constant=0.2;   % per ms

