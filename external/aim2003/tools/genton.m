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

function res=genton(nr_comps,nr_harm,f0,len)

if nargin < 4
    len=0.3;
end

s1=signal(len,16000); %erster Ton

fundamental=f0;
fcs=nr_harm*f0;
bandwidth=(nr_comps-1)*f0;
s=sprintf('sig=genharmonics(s1,''fundamental'',''%f'',''filterprop'',[%f 1 %f 1]);',fundamental,fcs,bandwidth);
eval(s);

sig=scaletomaxvalue(sig,1);
sig=RampAmplitude(sig,0.01); % baue eine Rampe

sig=setname(sig,sprintf('Fundamental:%dHz',round(f0)));
res=sig;