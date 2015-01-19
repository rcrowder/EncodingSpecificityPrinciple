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
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function sig=modtuningtilt(sig)

sig=antitilt(sig,0.04); % mache die Neigung rückgängig


sig=tilt(sig,0.08); % tilte, aber etwas weniger



vals=sig.werte;


cutoff=500; % Hz
drop=0.002; % Falle bis auf Null in 2 ms
durationtotilt=1/cutoff; 

tiltstart=time2bin(sig,-durationtotilt);  % Hier gehts los
tiltend=time2bin(sig,-durationtotilt+drop);        % und hier hörts schon wieder auf
tiltnr=tiltend-tiltstart;
ti=linspace(1,0,tiltnr);

vals(tiltstart+1:tiltend)=vals(tiltstart+1:tiltend).*ti(:);
vals(tiltend:end)=0;
% figure
% subplot(2,1,1)
% plot(sig);
sig.werte=vals;
% subplot(2,1,2)
% plot(sig);

