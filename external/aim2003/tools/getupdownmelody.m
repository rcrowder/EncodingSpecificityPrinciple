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

function fres=getupdownmelody(nr,aroundfre,upordown)

if nargin < 3
    upordown='down';
end
if nargin < 2
    aroundfre='A4';
end
if nargin < 1
    nr=5;
end


% notearray1=note2fre(1:83);
% notearray2=find(notearray1 > aroundfre & notearray1 < to);
% notearray=notearray1(notearray2);

fres=[];
df=power(2,1/12);
if strfind(upordown,'up')
    fre1=note2fre(aroundfre);
    fre2=fre1*power(df,2);
else
    fre1=note2fre(aroundfre);
    fre2=fre1/power(df,2);
end

for i=1:nr
    if mod(i,2)
        fres(i)=fre1;
    else
        fres(i)=fre2;
    end
end
    