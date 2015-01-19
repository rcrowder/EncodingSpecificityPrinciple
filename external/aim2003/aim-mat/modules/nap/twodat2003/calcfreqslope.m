%  
% function adjacent_band_level=calcfreqslope(cf,af);
%
%   INPUT VALUES:
% 		
%
%   RETURN VALUE:
%		
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Tim Ives (tim.ives@mrc-cbu.cam.ac.uk)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/04/28 10:07:54 $
% $Revision: 1.9 $

function adjacent_band_level=calcfreqslope(cf,af,b);
% function used to calculate the relative amplitudes in adjacent GTFBs
%e.g to calculate the level in 2000Hz critical band due to excitation 
%in 1000Hz critical band calcfreqslope(2000,1000);


n=4;
%ERB = 24.7+0.108.*cf;
[dummy ERB]=Freq2ERB(cf);
B=b.*ERB;


h_cf=abs(3.*(B./(2.*pi.*i.*(cf-cf)+2.*pi.*B)).^n);
h_af=abs(3.*(B./(2.*pi.*i.*(af-cf)+2.*pi.*B)).^n);

adjacent_band_level=h_af./h_cf;
adjacent_band_level=20.*log10(adjacent_band_level);


