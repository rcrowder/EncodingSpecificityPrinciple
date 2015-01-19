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

function contrast=getmaxcontrast(wo,maxpos,minpos,maxs,mins)
% usage: getmaxcontrast(wo,maxs,mins)
% returns the contrast of the maximum at wo
% this is calculated by taking the left and the right minimum of the max

[leftminwo,leftmin]=getminimumleftof(wo,maxpos,minpos,maxs,mins);
if isempty(leftminwo)
    % wenns keinen linkes minimum gibt, versuche, obs ein rechtes gibt
    [rightminwo,rightmin]=getminimumrightof(wo,maxpos,minpos,maxs,mins);
    if ~isempty(rightminwo)
        leftmin=rightmin;
    else
        leftmin=0;
    end
end

[rightminwo,rightmin]=getminimumrightof(wo,maxpos,minpos,maxs,mins);
if isempty(rightminwo)
    rightmin=leftmin;
end

maxval=maxs(find(maxpos==wo));

% Wenn der Punkt kein Maximum der Einhüllenden ist
if isempty(maxval)
    p=0;
%     error('getmaxcontrast:maximum not in list');
end

minval=(rightmin+leftmin)/2;

contrast=(maxval-minval)/(maxval+minval);

