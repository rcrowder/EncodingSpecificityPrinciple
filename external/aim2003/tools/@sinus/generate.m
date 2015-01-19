% method of class @sinus (derived from @signal)
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

function sinu=generate(sinu)


phase=sinu.phase;
amplitude=sinu.amplitude;
fre=sinu.frequency;

nr_points=getnrpoints(sinu);
sr=getsr(sinu);
length=getlength(sinu);

von=0+phase;
periode=1/fre;
bis=2*pi*length/periode + phase;

temp=linspace(von,bis,nr_points);
data=sin(temp);

data=data*amplitude;

sinu=setvalues(sinu,data);
