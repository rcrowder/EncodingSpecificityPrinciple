% method of class @sinus (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/02/01 14:15:26 $
% $Revision: 1.4 $

function sinu=sinus(length,samplerate,frequency,amplitude,phase)
% produces a sinus as a signal

if nargin < 5
    phase=0;
end
if nargin < 4
    amplitude=1;
end
if nargin < 3
    frequency=1;
end
if nargin < 2
    samplerate=1000;
end
if nargin < 1
    length=1;
end

name= sprintf('Sinus with %5.2f Hz',frequency);
unit_y='amplitude';
unit_x='time (ms)';
start_time=0;

ctt.frequency=frequency;
ctt.amplitude=amplitude;
ctt.phase=phase;

sig=signal(length,samplerate,name,unit_x,unit_y,start_time);

% f=class(struct([]),'fsignal',sig);
sinu=class(ctt,'sinus',sig);

sinu=generate(sinu);

