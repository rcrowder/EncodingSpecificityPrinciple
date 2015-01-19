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
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function ret=struct2double(str,field,mmm)
% returns a field of the values in str.fild
% like: ret=str(1:12).value

nr=length(str);
if nargin < 3
    % ret=zeros(nr,1);
    for i=1:nr
        eval(sprintf('dim=length(str(1).%s);',field));
        if dim==1
            eval(sprintf('ret(%d)=str(%d).%s;',i,i,field));
        else
            eval(sprintf('ret(%d,:)=str(%d).%s;',i,i,field));
        end
    end
else
    for i=1:nr
        eval(sprintf('ret(%d,:)=str(%d).%s(%d);',i,i,field,mmm));
    end
end