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

function res=resrictstructtohighest(str,topic,maxrange)


nr=length(str);

for i=1:nr
    sortcount(i)=eval(sprintf('str{%d}.%s',i,topic));
end

maxheight=max(sortcount);

count=1;
for i=1:nr
    if sortcount(i) > maxheight*(1-maxrange)
        res{count}=str{i};
    end
end



