% method of class @
% function newsig=squeeze(sig,factor) 
%   INPUT VALUES:
%  		sig: orginal @signal
%		factor: factor, by which the signal is squeezed
%   RETURN VALUE:
%		newsig: squeezed signal
% 
% reduces the length of the signal by scaling it in time
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function newsig=squeeze(sig,factor)


%%%%%%%%%%%%%%%
% if squeezed it becomes shorter
if factor < 1
    len=getlength(sig);
    newlen=len*factor;
    sr=sig.samplerate;
    newsr=sr;   % dont change the sr
    if newlen < newsr
        newlen=newsr;
    end
    newsig=signal(newlen,newsr);
    nr_points=getnrpoints(newsig);
    new_vals=zeros(nr_points,1);
    for i=1:nr_points
        whichstart=(i-1)*newsr/factor;
        whichstop=i*newsr/factor;
        
        if whichstop>len
            whichstop=len;
        end
        new_vals(i)=average(sig,whichstart,whichstop);    
    end
    newsig=setvalues(newsig,new_vals);
    return
end

newsig=sig;


