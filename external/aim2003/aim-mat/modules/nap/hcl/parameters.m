% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/25 15:43:01 $
% $Revision: 1.13 $
%%%%%%%%%%%%%
% hcl
% hidden parameters
hcl.generatingfunction='gen_hcl';
hcl.displayname='halfwave rectification, compression and lowpass filtering';
hcl.revision='$Revision: 1.13 $';

% parameters relevant for the calculation of this module
hcl.compression='log';
hcl.do_lowpassfiltering=1;
hcl.lowpass_cutoff_frequency=1200;
hcl.lowpass_order=2;
