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

function fres=getrandommelody(nr,from,to)

if nargin < 3
    to=1000;
end
if nargin < 2
    from=100;
end
if nargin < 1
    nr=5;
end


notearray1=note2fre(1:83);
notearray2=find(notearray1 > from & notearray1 < to);
notearray=notearray1(notearray2);

fres=[];
for i=1:nr
    randnr=ceil(rand(1)*size(notearray,2));
    note=notearray(randnr);
    fres(i)=note;
end
    