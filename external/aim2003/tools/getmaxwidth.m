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

function width=getmaxwidth(wo,maxpos,minpos,maxs,mins)
% usage: width=getmaxwidth(wo,frewomax,fremaxs,frewomin,fremins)
% returns the width of the maximum
% this is calculated by taking the left and the right minimum of the max

[leftminwo,val]=getminimumleftof(wo,maxpos,minpos,maxs,mins);
if isempty(leftminwo)
    % wenns keinen linkes minimum gibt, versuche, obs ein rechtes gibt
    [rightminwo,rightmin]=getminimumrightof(wo,maxpos,minpos,maxs,mins);
    if ~isempty(rightminwo)
        leftminwo=rightminwo;
    else
        leftminwo=0;
    end
end

[rightminwo,val]=getminimumrightof(wo,maxpos,minpos,maxs,mins);
if isempty(rightminwo)
    rightminwo=leftminwo;
end

width=abs((leftminwo+rightminwo)/2);

