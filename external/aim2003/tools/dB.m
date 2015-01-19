function x=dB(y,method)
%   convert to dB representation

if nargin<2
    method='power';
end

if strcmp(method,'power')
    x=10*log(y)./log(10);
else
    x=20*log(y)./log(10);
end
