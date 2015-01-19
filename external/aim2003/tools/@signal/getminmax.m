% method of class @signal
%
% returns the height and lows in locations and time of all local maxima in the signal
% in case of continuus maxima, the last value of the series is taken
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/29 17:36:18 $
% $Revision: 1.4 $

function [maxpos,minpos,maxs,mins]=getminmax(sig)
% usage: [maxpos,minpos,maxs,mins]=getminmax(sig)


werte=getdata(sig);
werte=werte';

% find all maxima
% mit NULL!!
maxpos = find((werte >= [0 werte(1:end-1)]) & (werte > [werte(2:end) 0]));
maxs=werte(maxpos);

% find all minima
minpos = find((werte < [inf werte(1:end-1)]) & (werte <= [werte(2:end) inf]));
mins=werte(minpos);

maxpos=bin2time(sig,maxpos);
minpos=bin2time(sig,minpos);