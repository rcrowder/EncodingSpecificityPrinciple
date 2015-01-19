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
% $Date: 2003/01/25 12:47:43 $
% $Revision: 1.4 $

function w=getcenterofmass(sig)

vals=getvalues(sig);
nr=getnrpoints(sig);

su1=0;
su2=0;

for i=1:nr
   su1=su1+vals(i)*i;
   su2=su2+vals(i);
end

if su2~=0
	w=su1/su2;
else
	w=nr/2;
end
	