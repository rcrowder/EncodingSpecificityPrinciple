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
% $Date: 2003/01/31 16:33:51 $
% $Revision: 1.2 $

function returnframes=genams(nap,strobes,options)
% loads in an aiff-file generated from the spf-file

str_model=options.spffile;
framespersecond=options.framespersecond;
output_normalization=options.output_normalization;
str_soundcommand=options.soundfile;

frames=getaiffs('modelfile',str_model,...
    'soundfile',str_soundcommand,...
    'framespersecond',sprintf('%f',framespersecond),...
    'output_normalization',sprintf('%f',output_normalization),...
    'echo','0');
	
%    'movie_duration',movie_duration,...
%    'movie_start_time',movie_start_time,...
	

nr=length(frames);
for i=1:nr
	tempframe=frames(i);
	% the ams-frames come with negative times. In order to present them
	% identical, we turn around the time axis:
	stt=getmaximumtime(tempframe);
	tempframe=reverse(tempframe);
	tempframe=setstarttime(tempframe,-stt);
	tempframe=getpart(tempframe,0,0.035);
	returnframes{i}=tempframe;
end