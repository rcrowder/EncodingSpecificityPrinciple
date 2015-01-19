function vals=getxvalues(sig)
% return all x values of the signal in one vector

sr=getsr(sig);
time_null=getminimumtime(sig);
time_max=getmaximumtime(sig);
vals=[time_null:1/sr:time_max];
% correction:
vals=vals(1:length(getvalues(sig)));
vals=vals';