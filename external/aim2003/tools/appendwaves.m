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

function sumwaves = appendwaves (varargin)

numwav=length(varargin);

for i=1:numwav
    sig(i)=loadwavefile(signal,varargin{i});
end

sumwaves=sig(1);
for i=2:numwav
    sumwaves=append(sumwaves,sig(i));
end
