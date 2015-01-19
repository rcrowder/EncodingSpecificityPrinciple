% function  result = [tc, sx, yexp] = interpol_exp(xinput, yinput)
%
%   Try to find the parameters of a exp function that fit best to the input
%   data
%
%
%   INPUT VALUES:
%               xinput, yinput:  sorted ascending X values 
%               
%             
%  
%   RETURN VALUE:
%               tc: half-life of exp fkt.
%               sx: x shift
%               yexp = y values of exp function
%               we consider there is no y shift
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/05/16 13:08:54 $
% $Revision: 1.1 $

function [tc, sx, yexp] = interpol_exp(xinput, yinput)


% apply logarithmic transformation
ylog = log(yinput);
p = polyfit(xinput, ylog, 1);
yexp = exp(p(1).*xinput + p(2));
% must be something wrong with tc, sx
tc = -1/p(1);
sx = -(p(2)- log(yinput(1)))/p(1);
