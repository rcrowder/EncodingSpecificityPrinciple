% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/11 10:44:50 $
% $Revision: 1.9 $
%%%%%%%%%%%%%
% strobes
% hidden parameters
sf2003.generatingfunction='gen_sf2003';
sf2003.displayname='strobe finding';
sf2003.revision='$Revision: 1.9 $';

% parameters relevant for the calculation of this module
sf2003.strobe_criterion='interparabola'; % can be 'parabola', 'bunt','adaptive'
% sf2003.unit='sec'; % which unit all the other parameters have

sf2003.strobe_decay_time=0.02;
% parameters for parabola:
sf2003.parabel_heigth=1.2;
sf2003.parabel_width_in_cycles=1.5;
% parameters for bunt:
sf2003.bunt=1.02;
sf2003.wait_cycles=1.5; % the time, that no new strobe is accepted
sf2003.wait_timeout_ms=20;

% parameters for 'adaptive':
sf2003.slope_coefficient=1;

