%  
% function sig=gen_multiramp(carriers,halflifes,reprate,signal_length, sample_fq)
%
%   Generates a superposition of ramped sinusiod signals
%
%   INPUT VALUES:
% 		carriers        carrier frequences (vector)
%       halflifes       (vector, same length as carriers)
%       reprate         frequence of envelope
%       signal_length   in seconds !!!
%       sample_fq       sample frequence 		
%
%   RETURN VALUE:
%		sig             object signal
%		
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 14:49:36 $
% $Version$

function sig=gen_multiramp(carriers,halflifes,reprate,signal_length, sample_fq)
if nargin < 4
    signal_length=0.128;
end
if nargin < 3
    reprate=62.5;   
end
if nargin < 2
    hlsteps=8;
    halflives=distributelogarithmic(0.064,0.0005,hlsteps);
else
    hlsteps=length(halflifes);
end
if nargin < 1
    carsteps=5;
    carriers=distributelogarithmic(250,4000,carsteps);
else
    carsteps=length(carriers);
end


sr = sample_fq;
% sr=16000;
% signal_length=0.264-1/sr;
sig=signal(signal_length,sr);

for i=1:hlsteps
    for j=1:carsteps
        current_carrier=carriers(j);
        halflife=halflifes(i);
        sig=generaterampsinus(sig,current_carrier,reprate,1,halflife);
        
        if j==1
            gsig=sig;
        else
            gsig=gsig+sig;
        end
    end
    
    if i==1
        tsig=gsig;
    else
        tsig=append(tsig,gsig);
    end
    
end
% savewave(tsig,'tsig');


sig=tsig;