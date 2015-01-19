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

function [pos_max,maxval,index]=getmaximumrightof(where,maxpos,minpos,maxs,mins)
% usage: [pos_max,maxval]=getmaximumrightof(where,womins,minvals,womaxs,maxvals)
% returns the maximum that is right of point "where"
% mins and maxs must be complete sets of minimums and maximums 
% interwoven

pos_max=[];
maxval=[];
index=-1;

if where > maxpos(end)
    return;
    error('getmaximumrightof:: no maximum right of point');
end


nr =length(where);
for j=1:nr
    cwhere=where(j);
    nr_maxs=length(maxpos);
    for i=1:nr_maxs
        if fround(maxpos(i),5) > fround(cwhere,5)
            pos_max(j)=maxpos(i);
            maxval(j)=maxs(i);
            index=j;
            break;
        end
    end
end

% nr_maxs=length(womaxs);
% for i=nr_maxs:-1:1
%     if womaxs(i) < where
%         pos_max=womaxs(i);
%         maxval=maxvals(i);
%         return;
%     end
% end

