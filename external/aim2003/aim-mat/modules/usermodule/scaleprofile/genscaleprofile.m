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
% $Date: 2003/07/27 15:29:59 $
% $Revision: 1.3 $

function usermodule=genscaleprofile(sai,options)

% find out about scaling:
maxval=-inf;
maxfreval=-inf;
maxsumval=-inf;

nr_frames=length(sai);
for ii=1:nr_frames
	maxval=max([maxval getmaximumvalue(sai{ii})]);
	maxsumval=max([maxsumval getscalesumme(sai{ii})]);
	maxfreval=max([maxfreval getscalefrequency(sai{ii})]);
end

waithand = waitbar(0,'Generating scale profile');
for frame_number=1:nr_frames
	usermodule{frame_number}= frame;
end
for frame_number=nr_frames:nr_frames
    
    waitbar(frame_number/nr_frames, waithand);     
    
	current_frame = sai{frame_number};
	current_frame = setallmaxvalue(current_frame, maxval);
	current_frame = setscalesumme(current_frame, maxsumval);
	current_frame = setscalefrequency(current_frame, maxfreval);
	
    usermodule{frame_number}= createscaleprofile(current_frame,options);
    usermodule{frame_number}=usermodule{frame_number}/100;
end
close(waithand);
