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

function str=readparameterfile(filename)

lines=loadtextfile(filename);
nr_lines=size(lines,2);

varcounter=1;
for i=1:nr_lines  % tranlsate each line in a parameter and a value
    line=lines{i};
    count=1;tab=sprintf('\t');ret=sprintf('\n');
    maxline=size(line,2);
    while ~strcmp(line(count),' ')  & ~strcmp(line(count),tab) & count < maxline  & count <1000 % run till the first space
        count=count+1;
    end
    str1=line(1:count-1);
    str2=line(count+1:end);
    if strcmp(str2,'')        str2=' ';    end
    
    str.name{varcounter}=str1;
    str.argument{varcounter}=str2;
    varcounter=varcounter+1;
end



