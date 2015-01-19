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

function str=getargument(source,search)
% usage: str=getargument(source,search)
% searchs in the pairs of parameters 'param','value' for the argument and returns the value as string
% source must be a even number of input parameters

% for compatibility with old version
if ~isfield(source,'name')
    nr=size(source,2);  % so many pairs
    for i=1:nr
        param=source(i);
        if strcmp(param,search)
            str= source(i+1);
            return
        end
    end
    str{1}='';
    return
   
else
    
    % new version is much more elegant
    nr=size(source.name,2);  % so many pairs
    for i=1:nr
        param=source.name{i};
        if strcmp(param,search)
            str=source.argument{i};
            
            % if the argument is put into "[]" then remove them
            return
        end
    end
end
str='';
return 


