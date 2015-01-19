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
% $Date: 2003/06/11 10:45:20 $
% $Revision: 1.5 $

function val=gettimevalue(sig,times)
% usage: val=gettimevalue(sig,time)
% returns the value at this point in time

val=zeros(size(times));
if isempty(times)
	return
end

sr=1/getsr(sig);

threshold=sr/100;
nr_points=getnrpoints(sig);
start=getminimumtime(sig);
stop=getmaximumtime(sig);
x=start+sr:sr:stop;
Y=sig.werte;
method='linear';


for ii=1:length(times);
	time=times(ii)-sig.start_time;
	
	nrint=round(time/sr);
	rint=time/sr;
	
	if (nrint-rint)<threshold
		if nrint>0 && nrint <= length(sig.werte)
			val(ii)=sig.werte(nrint);
		end
	else
		xi=times(ii);
		val(ii)=interp1(x,Y,xi,method);
	end
end


