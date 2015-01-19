
% (c) 2003, University of Cambridge, Medical Research Council 
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/05/06 13:38:34 $
% $Revision: 1.9 $
%

function nvals=leakyintegrator(sig,lowpass_cutoff_frequency,order)


time_constant=1/(2.*pi.*lowpass_cutoff_frequency);
sr=getsr(sig);
vals=getvalues(sig);
b=exp(-1/(sr.*time_constant));
gain=1./(1-b);


nvals=zeros(size(vals));
for dothis=1:order
	xn_1=0;
	yn_1=0;
    for i=1:length(vals)
        xn=vals(i);
        yn= xn + b*yn_1 ;
        xn_1=xn;
        yn_1=yn;
        nvals(i)=yn;
    end
    vals=nvals./gain;
end

nvals=vals;