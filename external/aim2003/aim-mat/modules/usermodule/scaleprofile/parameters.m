% parameter file for 'aim-mat'
% 
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/27 15:29:59 $
% $Revision: 1.2 $
%%%%%%%%%%%%%
% usermodule dual profile
% hidden parameters
scaleprofile.generatingfunction='genscaleprofile';
scaleprofile.displayname='scale profile';
scaleprofile.displayfunction='displayscaleprofile';
scaleprofile.revision='$Revision: 1.2 $';

% parameters relevant for the calculation of this module
scaleprofile.angle=45; % the angle in degree between frequency and interval
scaleprofile.min_harmonic_relationship=0.5; % lowest harmonic (last diagonal)
scaleprofile.max_harmonic_relationship=30;% highest harmonic (first diagonal)
scaleprofile.nr_points=100; % so many diagonals are generated