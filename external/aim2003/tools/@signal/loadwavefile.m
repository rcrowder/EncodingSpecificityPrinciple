% method of class @signal
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/15 09:48:33 $
% $Revision: 1.4 $

function [sig1,sig2]=loadwavefile(sig,orgname,timestart,duration)
% usage: [sig1,sig2]=loadwavefile(name,time)
% reads in the wavefile name in the time boundaries 
% returns two signals, if signal is stereo

% 
% [path,name,ext]=fileparts(name);
% if strcmp(ext,'.wav')
%     
%     [data,sr,nbits]=wavread(name);
% elseif strcmp(ext,'.aif') || strcmp(ext,'.aiff')
%     error('aif import not implemented yet');
% end
% 
% sig=signal(data);
% sig=setsr(sig,sr);
% sig=setname(sig,name);
% if nargin > 2
%     sig=getpart(sig,timestart,timestart+duration);
% end
% 

[name,path,ext]=fileparts(orgname);
if strcmp(ext,'.wav')
    try
        [data,sr]=audioread(orgname);
    catch
        error(sprintf('cant open file %s',orgname))
    end
    nr_sig=size(data,2);
    
    sig1=signal(data(:,1));
    sig1=setsr(sig1,sr);
    sig1=setname(sig1,orgname);
    if nargin > 2
        sig1=getpart(sig1,timestart,timestart+duration);
    end
    if nr_sig==2
        sig2=signal(data(:,2));
        sig2=setsr(sig2,sr);
        sig2=setname(sig2,orgname);
        if nargin > 2
            sig2=getpart(sig2,timestart,timestart+duration);
        end
        
    end
elseif strcmp(ext,'.aif') || strcmp(ext,'.aiff')
    [data,sr]=aifread(orgname);
    sig=signal(data);
    sig=setsr(sig,sr);
    sig=setname(sig,orgname);
    if nargin > 2
        sig=getpart(sig,timestart,timestart+duration);
    end
end