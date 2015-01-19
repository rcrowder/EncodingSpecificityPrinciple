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
% Christoph Lindner
% $Date: 2003/06/27 17:52:54 $
% $Revision: 1.5 $

function displaytunedDP(sai,options,frame_number)
show_time = 0;


% Test if the frame_number is available
if length(sai)<frame_number
    % no its not
    return
end
if ~(isfield(sai{frame_number}, 'interval_sum') & isfield(sai{frame_number}, 'frequency_sum'))
    return
end
if ~(isfield(sai{frame_number}, 'peaks_interval_sum') & isfield(sai{frame_number}, 'peaks_frequency_sum'))
    return
end

% ?????? per Definition ????
minimum_time_interval=0.1;  % in ms
maximum_time_interval=32;

fq_sum = sai{frame_number}.frequency_sum;
int_sum = sai{frame_number}.interval_sum;
if options.smooth_sigma_FP
    fq_sum = smooth(fq_sum, options.smooth_sigma_FP);
end        

peaks_tip = sai{frame_number}.peaks_interval_sum;
% % -x-x-x-x-x-x-x-x-x-x-x-x-x-   
% %Experimental -- this is the draft reduction
% %Get rid of the first 0s
% tip = getvalues(int_sum);
% for i=1:length(tip)
%     startpoint=i;
%     if tip(i)~=0
%         break
%     end
% end
% binvalue=bin2time(int_sum,startpoint);
% if binvalue~=getnrpoints(int_sum)
%     tip = getvalues(getpart(int_sum,bin2time(int_sum,startpoint))) - ...
%           getvalues(smooth(getpart(int_sum,binvalue),getnrpoints(int_sum)/16));
% end
% tip = tip - min(tip);
% tip = tip/max(tip);
% new_intsum = setvalues(int_sum, tip, startpoint);
% p.dyn_thresh = 0;
% p.smooth_sigma = 1;
% peaks_tip = PeakPickertunedDP(new_intsum, p);
% int_sum = new_intsum;
% -x-x-x-x-x-x-x-x-x-x-x-x-x-   
     
% Analyse the dualprofile
apTIP=options.aprioriTIPpeakfq;
apFQP=options.aprioriFQPpeakfq;
r = analyse_aim_profilestunedDP(int_sum, fq_sum, peaks_tip, sai{frame_number}.peaks_frequency_sum, sai{frame_number}.channel_center_fq,apTIP,apFQP);

%r = sai{frame_number}.pitch_strength_prediction;
% Plot the results
r.fqp=r.fqp*options.tuningFactor;
disp('---------------------------------------------------------------------------------------');
disp(sprintf('Relative spectral pitch strength: %1.5f,   centre frequency of the highest peak: %4.0f Hz', r.fqp, r.dfq));
disp(sprintf('Relative residue pitch strength:  %1.5f,   frequency of residue peak:            %4.0f Hz', r.tip, r.dti));
disp('Remark: consider a priori option in parameter file if wrong residue peak is found!');
r.ratio=r.tip/r.fqp;
disp(sprintf('TIP/FQP: %4.6f', r.tip/r.fqp));
% save('tunedDP_tmp',r);
cla;
%Plot both profiles into one figure
% FQP
fqp = getvalues(fq_sum)';
plot(sai{frame_number}.channel_center_fq, fqp,'r');
hold on

% TIP
tip=getvalues(int_sum);

tip_x = bin2time(sai{frame_number}.interval_sum, 1:length(tip));  % Get the times
tip_x = tip_x((tip_x>=(minimum_time_interval/1000)) & tip_x<=(maximum_time_interval/1000));  
tip = tip(time2bin(sai{frame_number}.interval_sum,tip_x(1)):time2bin(sai{frame_number}.interval_sum,tip_x(end)));
% tip_x is in ms. Change to Hz
tip_x = 1./tip_x;
plot(tip_x, tip, 'b');
set(gca,'XScale','log');    


% Now lable it !
xlabel('Frequency [Hz]');
% ylabel('To Do');
% title('The Dual Profile');
set(gca, 'YAxisLocation','right');
pks = [];
% ----- Label the highes Peaks
% if length(sai{frame_number}.peaks_interval_sum)>0
%     pks = 1/bin2time(sai{frame_number}.interval_sum, sai{frame_number}.peaks_interval_sum{1}.x);
% end
% if length(sai{frame_number}.peaks_frequency_sum)>0
%     pks = [pks sai{frame_number}.channel_center_fq(sai{frame_number}.peaks_frequency_sum{1}.x)];
% end
% ----- Label the results
% if r.dfq>0
%     pks = r.dfq;
% end
% if r.dti>0
%     pks = [pks r.dti];
% end

% ----- plot frequencys on top of the results
if r.dfq>0
    plot(r.dfq, fqp(find(sai{frame_number}.channel_center_fq==r.dfq)),'r.');
    text(r.dfq, fqp(find(sai{frame_number}.channel_center_fq==r.dfq)), [num2str(round(r.dfq)) 'Hz'],'VerticalAlignment','bottom','HorizontalAlignment','center');
end
if r.dti>0
     plot(r.dti, tip(find(tip_x==r.dti)),'b.');
     text(r.dti, tip(find(tip_x==r.dti)), [num2str(round(r.dti)) 'Hz'],'VerticalAlignment','bottom','HorizontalAlignment','center');
end

nr_labels = 8;
ti=50*power(2,[0:nr_labels]);
for i=1:length(pks)
    ti = ti((ti>(pks(i)+pks(i)*0.1))|(ti<pks(i)-pks(i)*0.1));
end
ti = [ti round(pks)];
ti = sort(ti);
set(gca,'XTick', ti);
set(gca, 'XLim',[1000/maximum_time_interval sai{frame_number}.channel_center_fq(end)])
set(options.handles.checkbox6, 'Value',0);
set(options.handles.checkbox7, 'Value',0);
hold off


% set(gca, 'YLim',[0 2000])

return




% ------------ subfunctions ---------------------

% turns a vector (row) upside down
function y=upsidedown(x)
y=[];
for i=length(x):-1:1
    y=[y x(i)];
end
