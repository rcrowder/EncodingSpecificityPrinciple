% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/03/19 10:44:33 $
% $Revision: 1.1 $

function sig=genome(sig,options)
% generating function for the outer/middle ear transfere function 

waithand=waitbar(0,'generating pre cochlea processing');

samplerate=getsr(sig);
start_time=getminimumtime(sig);


		firfiltercoef = OutMidCrctFilt('MAP',samplerate,0);


% to compensate the huge delay from the filter, a pause is added to the
% signal, that is taken away in the end:
pause=signal(abs(options.delay_correction),samplerate);
sig=setstarttime(sig,0);
sig=append(sig,pause);

Snd=getvalues(sig);
Snd=Snd';
Snd = filter(firfiltercoef,1,Snd);

sig=setvalues(sig,Snd);

%take the delay back:
siglen=getlength(sig);
sig=getpart(sig,abs(options.delay_correction),siglen);
sig=setstarttime(sig,start_time);

close(waithand);