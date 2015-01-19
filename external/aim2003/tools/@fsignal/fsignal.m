% method of class @fsignal (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:47 $
% $Revision: 1.3 $

function f=fsignal(laenge,samplerate)

start_time=0;
unit_y='amplitude (dB)';
unit_x='Frequency (Hz)';
name='generic Frequencysignal';
if nargin< 2
    samplerate=1000;
end

% Konstruktor kann auch ein Vektor sein, dann wird direkt konstruiert.
lang=size(laenge,1);
if size(laenge,2)>1 %Benutzer hat falschen Vektor (Zeilenvektor) eingegeben
    lang=size(laenge,2);
    laenge=laenge';
end
if lang>1 % AHA! Ein Vektor
    f.werte=laenge;
    nr_points=lang;
else
    nr_points=laenge;
    f.werte=zeros(round(nr_points),1);
end

f.max_fre=samplerate;
f.df=samplerate/nr_points;     %frequenzabstand

sig=signal(laenge,samplerate,name,unit_x,unit_y,start_time);

f=class(f,'fsignal',sig);

