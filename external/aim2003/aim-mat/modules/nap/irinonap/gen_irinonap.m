% generating function for 'aim-mat'
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Revision: 1.2 $

function nap=gen_irinonap(bmm,options)

%waithand=waitbar(0,'generating NAP');

%setting up the parameters used
bias = 0.000001; %is usually 0.1
NAPparam.SubBase  = 0.5;

%converting the BMM frame into a matrix
bmm_matrix=getvalues(bmm);   

%or if you wish to use a stored version
%load bmm_irino.mat;
%bmm_matrix=BMM;

NumCh=size(bmm_matrix);
samplerate=getsr(bmm);

for nch = 1:NumCh(1)
NAP_matrix(nch,:) = log10(max(bmm_matrix(nch,:),bias)) - log10(bias);
end;

if NAPparam.SubBase ~= 0,
disp([ '=== Baseline subtraction :  Max NAP = ' num2str( max(max(NAP_matrix))) ... 
	', NAPparam.SubBase = ' num2str(NAPparam.SubBase) ' ===']);
end;

NAP0_matrix = NAP_matrix;
NAP_matrix = max((NAP0_matrix - NAPparam.SubBase),0);

NAPparam.height = max(max(NAP_matrix));
%NAPparam.tms    = (0:LenSnd-1)/fs*1000;

%NAP_matrix=NAP_matrix/NAPparam.height;

%disp(NAP_matrix);
nap=frame(NAP_matrix);
nap=setsr(nap,samplerate);

return