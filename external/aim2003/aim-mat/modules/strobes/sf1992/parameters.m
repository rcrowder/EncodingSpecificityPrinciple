% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/04/14 13:12:57 $
% $Revision: 1.6 $
%%%%%%%%%%%%%
% old strobe finding
% hidden parameters
sf1992.generatingfunction='gen_sf1992';
sf1992.displayname='strobe finding old ams version';
sf1992.revision='$Revision: 1.6 $';

% parameters relevant for the calculation of this module

% can be 'peak','temporal_shadow','local_maximum','delta_gamma'
sf1992.strobe_criterion='temporal_shadow'; 
% parameter for temporal_shadow
sf1992.strobe_decay_time=0.02;


% which unit is applied to the time measurements below
% can be sec,ms,cycles:
sf1992.unit='sec';
% parameter for local_maximum
sf1992.strobe_lag=0.005;
sf1992.timeout=0.01;