function sig=createfrompoly(sig,p)
% creates the signal from the points in the polynoms p (very useful with
% polynomes that come from a fit

sr=getsr(sig);
xvals=getxvalues(sig);
pvals=polyval(p,xvals);
sig=setvalues(sig,pvals);