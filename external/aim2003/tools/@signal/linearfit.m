function [m,b]=linearfit(sig)
% fit the signal with a streight line and return the slope (m) and the zero
% crossing (b)

y=getvalues(sig);
x=getxvalues(sig);
[p,s] = polyfit(x,y,1);

m=p(1);
b=p(2);
