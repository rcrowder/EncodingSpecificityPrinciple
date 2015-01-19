% method of class @signal
% function sig=generategauss(sig,pos,hight,sigma)
%   INPUT VALUES:
%       sig: original @signal with length and samplerate 
%       pos: medium of the curve
%       hight: its height
%       sigma: and its standart derivation
% produces a gaussian bell curve with the parameters
% 
%   RETURN VALUE:
%       sig:  @signal 
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:42 $
% $Revision: 1.3 $

function sig=generategauss(sig,pos,hight,sigma)

vals=1:getnrpoints(sig);

evals=exp(-(pos-vals).^2/(2*sigma^2));

evals=evals*hight;

sig=setvalues(sig,evals);

