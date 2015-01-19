% tool
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:44 $
% $Revision: 1.3 $

function sig=dots2sig(dots,nr_bins)
% usage: sig=dots2sig(dots)
% makes a signal from the dots by adding them to bins according to their octave shift
% the dots must have pitchstrength in the region from 0 to 1
% the outsignal has nr_bins points and a length of 1


if nargin < 2
    nr_bins=100;
end

sig=signal(1,nr_bins);

nr=max(size(dots));
for i=1:nr
    shift=dots{i}.octave_shift;
    old_val=gettimevalue(sig,shift);
    new_val=dots{i}.pitchstrength;
    sig=settimevalue(sig,shift,old_val+new_val);
end
