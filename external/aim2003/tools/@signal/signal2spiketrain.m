% method of class @signal
% function st=signal2spiketrain(sig)
%   INPUT VALUES:
%       sig:       original @signal
%   RETURN VALUE:
% 		st: @spiketrain
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function st=signal2spiketrain(sig)
% translates a signal in an spiketrain

nr=getnrpoints(sig);
vals=getvalues(sig);

%spks=zeros(1000,1);

count=1;
for i=1:nr
    if vals(i)>0
        for j=1:vals(i)
            %st=addspike(st,vals(i));
            spks(count)=bin2time(sig,i);
            count=count+1;
        end
    end
end


st=spiketrain(spks);



