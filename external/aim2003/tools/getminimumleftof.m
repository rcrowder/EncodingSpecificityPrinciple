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
% $Date: 2003/06/11 10:45:20 $
% $Revision: 1.5 $

function [pos_min,minval,index]=getminimumleftof(where,maxpos,minpos,maxs,mins)
% usage: [pos_min,minval]=getminimumleftof(where,womins,minvals,womaxs,maxvals)
% returns the minimum that is left of point "where"
% mins and maxs must be complete sets of minimums and maximums 
% interwoven

pos_min=[];
minval=[];
index=-1;

if isempty(maxpos)
    return;
end
if where < minpos(1)
    return;
    error('getminimumleftof:: no minimum left of point');
end

nr =length(where);
for j=1:nr
    cwhere=where(j);
    nr_mins=length(minpos);
    for i=nr_mins:-1:1
%         if fround(minpos(i),5) < fround(cwhere,5)
%             pos_min(j)=minpos(i);
%             minval(j)=mins(i);
%             break;
%         end
        if minpos(i) < cwhere
            pos_min(j)=minpos(i);
            minval(j)=mins(i);
            index=i;
            break;
        end
    end
end
