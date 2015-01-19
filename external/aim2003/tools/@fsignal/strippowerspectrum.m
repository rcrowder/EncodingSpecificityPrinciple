% method of class @fsignal (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:47 $
% $Revision: 1.3 $

function nsig=strippowerspectrum(sig)
% usage: sig=strippowerspectrum(sig)
% returns a new fsignal, that has one fewer point and strips of the leading
% value, because the powerspectrum adds a zero value in front


val=getdata(sig);
valneu=val(2:end);  

nsig=fsignal(valneu);

nsig=setsr(nsig,getsr(sig));
nsig=setdf(nsig,getdf(sig));
nsig=setmaxfre(nsig,getmaxfre(sig));

nsig=setname(nsig,getname(sig));