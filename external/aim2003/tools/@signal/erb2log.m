% method of class @signal
% function sig=erb2log(sig,cfs)
%
% changes the signal (that is ususally a function of frequencies, into
% another signal with frequencies, but now the spacing is according to the
% cf list in cfs. All points are interpolated
%
%   INPUT VALUES:
%       sig: original @signal
%       cfs: list of frequencies
% 
%   RETURN VALUE:
%       sig: new @signal
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/20 18:35:23 $
% $Revision: 1.4 $

function sig=erb2log(sig,cfs)


min_fre=cfs(1);
max_fre=cfs(end);
nr_points=length(cfs);

log_fres=distributelogarithmic(min_fre,max_fre,nr_points);

erb_fres=cfs;
erb_vals=sig.werte;

method='cubic';
% for i=1:nr_points
%     log_fre=log_fres(i);
new_vals=interp1(erb_fres,erb_vals,log_fres,method);


% end

sig=setvalues(sig,new_vals);

a=0;



