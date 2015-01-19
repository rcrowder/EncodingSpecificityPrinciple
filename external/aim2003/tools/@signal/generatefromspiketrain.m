function sig=generatefromspiketrain(sig,st)

vals=zeros(size(getvalues(sig)));
times=time2bin(sig,st);

maxval=length(vals);
selecttimes=times(find(times>0 & times<maxval));
% sig=addtimevalue(sig,selecttimes,1);
vals(round(selecttimes))=vals(round(selecttimes))+1;

sig=setvalues(sig,vals);

