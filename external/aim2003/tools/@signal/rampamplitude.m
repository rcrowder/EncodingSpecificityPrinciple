% method of class @signal
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function sig=rampamplitude(sig,rt,type)

if nargin < 3
    type='linear';
end
if nargin < 2
    rt=0.01;
end

if strcmp(type,'linear')
    first_bin=1;
    nr_point=time2bin(sig,rt);
    last_bin=nr_point;
    if last_bin >= first_bin
        ramp=linspace(0,1,(last_bin-first_bin));
        sig=sig*ramp;
    end
    first_bin=getnrpoints(sig)-nr_point;
    start_time_hintere_rampe=bin2time(sig,first_bin);
    last_bin=time2bin(sig,getlength(sig));
    if (last_bin >= first_bin)
        damp=linspace(1,0,(last_bin-first_bin));
        try
            sig=mult(sig,damp,start_time_hintere_rampe,rt);
        end
    end
    p=1;
end
