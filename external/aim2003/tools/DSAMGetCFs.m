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

function ret=DSAMGetCFs(content_spfmodel)
% ret=DSAMGetCFs(in,fromhere)
% reads in the cfs from fromhere (usually: 'CENTRE_FREQ.BM_GammaT')
% and the number of channels from 'CHANNELS.BM_GammaT'
% change these, if not appropriate!


channelindikator='CHANNELS.BM_GammaT';
cfindikator='CENTRE_FREQ.BM_GammaT';


nrfreqstr=DSAMFindParameter(content_spfmodel,channelindikator);
nrcfs=sscanf(nrfreqstr,'%f');
ret=zeros(nrcfs,1);


nr=length(content_spfmodel);
for i=1:nr
    a=strfind(content_spfmodel{i},cfindikator);
    if a>0  % found the first indicator. From here, the next nr lines hold the information
        for j=1:nrcfs
            cfstr=content_spfmodel{i+j-1};
            cfstr=searchcfstr(cfstr);
            ret(j)=sscanf(cfstr,'%f'); % so many frequencies on y-axis
        end
        return        
    end
end



function ret=searchcfstr(line)
%principle: search for spaces. The value is the thing after the search string between the next spaces
%count=length(diesen)-1;  %startoint for the investigation
wo=findstr(line,':');
i=1;
tab=sprintf('\t');
next=line(wo+1);
while ~strcmp(next,' ') & ~strcmp(next,tab) & i<10
    ret(i)=next;
    i=i+1;
    next=line(wo+i);
end
