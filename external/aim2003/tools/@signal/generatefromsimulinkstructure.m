% method of class @signal
% function sig=generatefromsimulinkstructure(sig,struct)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       struct: struct, that is exported from simulink
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=generatefromsimulinkstructure(sig,struct)

%sr=struct.time(2)-struct.time(1);
sr=1;
len=length(struct.signals(1).values)*sr;
vals=struct.signals(1).values;

if size(vals,1)>1 & size(vals,2) > 1
    vals=vals(end,:);
end

sig=signal(size(vals,2),1/sr);
sig=setvalues(sig,vals);

%sig=signal(vals);
%sig=setsr(sig,1/sr);
sig=setname(sig,struct.blockName);

