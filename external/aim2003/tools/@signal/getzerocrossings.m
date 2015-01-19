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

function t=getzerocrossings(signal,var)
% gets the zero crossings of the signal.
% in Case, the Signal has many zeros and only few 
% bumps, like in a klicktrain, only the last zero is counted
% if var is there, then not real zero, but a value of var is taken

if nargin < 2 
    var=0;
end

werte=getdata(signal);

nr= getnrpoints(signal); % so many points
a=0;
count=1;
sr=getSR(signal);
for i=1:nr
    b=werte(i);
    if a <= var & b > var
        t(count)=bin2time(signal,i);
        count=count+1;
    end
    % shift the last values
    a=b;
end
