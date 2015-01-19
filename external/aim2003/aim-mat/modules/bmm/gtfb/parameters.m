% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/09/03 15:28:59 $
% $Revision: 1.6 $
%%%%%%%%%%%%%
% bmm
% hidden parameters
gtfb.generatingfunction='gen_gtfb';
gtfb.displayname='Gamma tone filter bank';
gtfb.revision='$Revision: 1.6 $';

% parameters relevant for the calculation of this module
gtfb.nr_channels=75;
gtfb.lowest_frequency=100;  % in Hz
gtfb.highest_frequency=6000;   % in Hz
gtfb.do_phase_alignment='off';	%'off','maximum_envelope','nr_cycles'
gtfb.phase_alignment_nr_cycles=3; % only relevant when do_align is nr_cylcles
gtfb.b=1.019; % Filterbandwidth standard: 1.019
