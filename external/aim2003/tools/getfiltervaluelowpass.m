% tool
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:44 $
% $Revision: 1.3 $

function scaler=getfiltervaluehighpass(cur_fre,cutoff,dbperoctave)


%feststellen, wieviel octaven ich über dem cutoff bin:
octs=log2(cur_fre/cutoff);

% wieviel dB sind das?
nrdB=octs*dbperoctave;

scaler=power(10,(-nrdB/20));

% gib nur die Werte > Cutoff zurück
scaler=min(1,scaler);






