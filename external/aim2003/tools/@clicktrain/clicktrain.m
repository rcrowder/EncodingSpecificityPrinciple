% constructor of class @clicktrain (derived from @signal)
% parent class: @signal
% function ct=clicktrain([length],[samplerate],[frequency],[amplitude])
 
%   INPUT VALUES:
%       length: length of signal in seconds [default 1]
%       samplerate: samplerate in Hz (points per seconds) [default: 1000]
%       frequency: frequency in Hz [default: 100]
%       amplitude: clicks amplitude [default: 1]
%   RETURN VALUE:
% 		@signal-object
%
%
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:46 $
% $Revision: 1.3 $



function ct=clicktrain(length,samplerate,frequency,amplitude)
% produces a clicktrain as a signal

if nargin < 4
    amplitude=1;
end
if nargin < 3
    frequency=100;
end
if nargin < 2
    samplerate=10000;
end
if nargin < 1
    length=1;
end

name= sprintf('Clicktrain with %5.2f Hz',frequency);
unit_y='amplitude';
unit_x='time (ms)';
start_time=0;

ctt.frequency=frequency;
ctt.amplitude=amplitude;

sig=signal(length,samplerate,name,unit_x,unit_y,start_time);

% f=class(struct([]),'fsignal',sig);
ct=class(ctt,'clicktrain',sig);

ct=generate(ct);

