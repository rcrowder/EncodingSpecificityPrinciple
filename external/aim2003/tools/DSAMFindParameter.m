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

function ret=DSAMFindParameter(in,diesen,nr_occurance)
% ret=DSAMFindParameter(in,diesen,nr)
% finds the parameter value of the parameter "diesen"
% eg:   NUM_RUNS.ams.0           	2         	Number of repeat runs, or segments/frames.
% returns "2"
% 
% nr_occurance is the repeated number of this line

if nargin < 3
    nr_occurance=1; %take the first
end

len=size(diesen,2);
new=in;


nr=size(in,2);
occurance=0;
linenr=0;
for i=1:nr
    a=strfind(in{i},diesen);
    if a>0
        occurance=occurance+1;
        if occurance>=nr_occurance
            linenr=i;
            break
        end
    end
end

if linenr==0
%    disp('DSAMFindParameter: Line not found');
    ret=' ';
    return;
end

% and now find the parameter
line=in{linenr};

%principle: search for spaces. The value is the thing after the search string between the next spaces
count=length(diesen)-1;  %startoint for the investigation
tab=sprintf('\t');
while ~strcmp(line(count),' ')  & ~strcmp(line(count),tab)  & count <1000 % run till the first space
    count=count+1;
end

count=count+1;

while (strcmp(line(count),' ') | strcmp(line(count),tab)) & (count <1000) % run till the first non space afterwards
    count=count+1;
end

nr=1;
while ~strcmp(line(count),' ') & ~strcmp(line(count),tab) & count <1000 % run till the first space - here is the information
    ret(nr)=line(count);
    nr=nr+1;
    count=count+1;
end

if count>999
    disp('DSAMFindParameter: Error: Something wrong with line:');
    disp(diesen);
end


