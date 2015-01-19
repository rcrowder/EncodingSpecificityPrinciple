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
% $Date: 2003/06/27 16:03:30 $
% $Revision: 1.6 $

function displaydualprofile(sai,options,frame_number)

% Test if the frame_number is available
if length(sai)<frame_number
    % no its not
    return
end
if ~(isfield(sai{frame_number}, 'interval_sum') & isfield(sai{frame_number}, 'frequency_sum'))
    return
end

% ?????? per Definition ????
minimum_time_interval=options.minimum_time_interval;  % in ms
maximum_time_interval=options.maximum_time_interval;
nr_labels = options.nr_labels;

% Normalize the profiles
fq_sum = sai{frame_number}.frequency_sum;
int_sum = sai{frame_number}.interval_sum;
if (getnrpoints(fq_sum)~=0)
    int_sum = int_sum/getnrpoints(fq_sum);
end
if (getnrpoints(int_sum)~=0)
    fq_sum = (fq_sum/getnrpoints(int_sum))*options.scalefactor*1.7;
end

cla;
%Plot both profiles into one figure
% frequency profile
fqp = getvalues(fq_sum)';
plot(sai{frame_number}.channel_center_fq, fqp,'r');
hold on

% time interval profile
tip=getvalues(int_sum);
tip_x = bin2time(sai{frame_number}.interval_sum, 1:length(tip));            % Get the times
tip_x = tip_x((tip_x>=(minimum_time_interval/1000)) & tip_x<=(maximum_time_interval/1000));  
tip = tip(time2bin(sai{frame_number}.interval_sum,tip_x(1)):time2bin(sai{frame_number}.interval_sum,tip_x(end)));
% tip_x is in ms. Change to Hz
tip_x = 1./tip_x;
plot(tip_x, tip, 'b');
set(gca,'XScale','log');    

% Now lable it !
xlabel('Frequency [Hz]');
set(gca, 'YAxisLocation','right');
ti=50*power(2,[0:nr_labels]);
set(gca,'XTick', ti);
set(gca, 'XLim',[1000/maximum_time_interval sai{frame_number}.channel_center_fq(end)])
set(options.handles.checkbox6, 'Value',0);
set(options.handles.checkbox7, 'Value',0);
hold off


return
