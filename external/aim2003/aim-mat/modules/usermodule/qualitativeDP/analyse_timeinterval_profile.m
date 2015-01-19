% function [result, dominant_ti] = analyse_timeinterval_profile(ti_profile, peaks, a_priori, fqp_fq)
%
%   To analyse the time interval profile of the auditory image
%
%   INPUT VALUES:
%           ti_profile      time-interval profile (signal class)
%           peaks           output of peakpicker 
%           a_priori        a_priori information - not used
%           fqp_fq          result of frequency profile - not used 
%
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/27 17:52:54 $
% $Revision: 1.3 $
function [result, dominant_ti] = analyse_timeinterval_profile(ti_profile, peaks, a_priori, fqp_fq,  apTIP)

% for debug resons
plot_switch = 0;

intsum = ti_profile;   
intsum_vals = getdata(intsum);
if plot_switch
    minimum_time_interval=0;  % in ms
    maximum_time_interval=100;
    tipfig = 14;
    savecf = gcf;
    figure(tipfig)
    hold off
    tip = intsum_vals;
    tip_x = bin2time(ti_profile, 1:length(tip));  % Get the times
    tip_x = tip_x((tip_x>=(minimum_time_interval/1000)) & tip_x<=(maximum_time_interval/1000));  
    tip = tip(time2bin(ti_profile, tip_x(1)):time2bin(ti_profile, tip_x(end)));
    % tip_x is in ms. Change to Hz
    tip_x = 1./tip_x;
    plot(tip_x, tip, 'b');
    set(gca,'XScale','log');    
    hold on
end
if length(peaks)==0
    % If there is no peak than we just plot the function
    result = 0;
    dominant_ti = 0;
    return
end

% ++++++++++++ Part one: Peak finding ++++++++++
% Calculate the envelope (curve of the peaks)

% sort peaks from low time interval to a heigh one
% or from left to right
peaks_lr = peaks;  % The sorte peaks
for i=1:(length(peaks_lr)-1)
    for j=length(peaks_lr):-1:i
        if (peaks_lr{i}.x>peaks_lr{j}.x)
            tmp = peaks_lr{i};
            peaks_lr{i}=peaks_lr{j};
            peaks_lr{j}=tmp;
        end
    end
end

if plot_switch
    px=[];py=[];
    for i=1:length(peaks)
        px = [px tip_x(peaks{i}.x)];
        py = [py tip(peaks{i}.x)];
    end
    plot(px,py,'kx');
end

% Create envelope of peaks
peak_envX = [];
peak_envY = [];
for i=1:length(peaks_lr)
    peak_envX = [peak_envX peaks_lr{i}.x];
    peak_envY = [peak_envY peaks_lr{i}.y];
end

if plot_switch
     plot(tip_x(peak_envX),  peak_envY, ':k');
end

% % Find the fitting exp Function
% [tc, sx, yexp] = interpol_exp(peak_envX, peak_envY);
% if plot_switch
%    x_exp = 1:length(intsum_vals); 
%    plot(peak_envY(1).*exp(-(x_exp-sx)./tc),'k:')
%    %plot(peak_envX, yexp,'k:')
% end
% % Idea: Find highes peak to exp-Fkt ration if above threshold
%  


% Find Maxima of the envelope 
% create signal 
peak_envsig = signal(length(peak_envX), 1);
peak_envsig = setvalues(peak_envsig, peak_envY);
params = 0; 
peak_env_peaks = PeakPicker(peak_envsig, params);
% sort peaks of the envelope from low time interval to a heigh one
% or from left to right
peaks_env_peaks_lr = peak_env_peaks;  % The sorte peaks
for i=1:(length(peaks_env_peaks_lr)-1)
    for j=length(peaks_env_peaks_lr):-1:i
        if (peaks_env_peaks_lr{i}.x>peaks_env_peaks_lr{j}.x)
            tmp = peaks_env_peaks_lr{i};
            peaks_env_peaks_lr{i}=peaks_env_peaks_lr{j};
            peaks_env_peaks_lr{j}=tmp;
        end
    end
end

if plot_switch
    for i=1:length(peaks_env_peaks_lr)
        %plot(tip_x(peak_envX(peaks_env_peaks_lr{i}.x)),  peak_envY(peaks_env_peaks_lr{i}.x), 'x');
    end
end


% % Now make sure, that highest peak is not the first peak of envelope
% peak_oi = peaks{1};
% if (peak_oi.x == peak_envX(peaks_env_peaks_lr{1}.x))
%     % The first Peak is the heighest -> take second highest of envelope
%     if (length(peak_env_peaks)>=2)
%          poix = peak_envX(peak_env_peaks{2}.x);
%          poiy = peak_env_peaks{2}.y;
%          for i=1:length(peaks)
%              if poix==peaks{i}.x
%                 peak_oi = peaks{i};  
%              end
%          end
%      end
% end

% Stefan's method on HCL
% Take second peak of envelope from short time intervals
if length(peaks_env_peaks_lr)>1
    for i=1:length(peaks)
        % If second highes peak==peak with smallest time intervall -> take
        % third highest !!
        if peak_env_peaks{2}.x == peaks_env_peaks_lr{1}.x
          poix = peak_envX(peak_env_peaks{3}.x);
        else
          poix = peak_envX(peak_env_peaks{2}.x);
        end
        if peaks{i}.x==poix
            peak_oi = peaks{i};
        end
    end
else
    % pure sinusoid ???
    dominant_ti = 0
    result = 0.001;
    return
end

if plot_switch
    plot(tip_x(peak_oi.x), peak_oi.y, 'ro');
end





% peak_oi contains the peak for quantification


% ++++++++++++ Part two: Quantification ++++++++++

% % Third Method:
% Peak to mean valley ration - good with HCL
result= 1 - peak_oi.left.y/peak_oi.y;
dominant_ti = peak_oi.x /getsr(ti_profile);

% Adaptation
%result = 1-result;

% +++++++++++++ Histogram of distances between two peaks ++++++++++

%
max_thresh = 1; %0.98;
% The linear TIP 

% Test if all other Peaks ar multiple of the highest peak

% Start with the first peak
% Test how many peaks are mutiple of the first peak
%---x---x---x---x---x---x
% Take first peak of envelope.
poix = peak_envX(peak_env_peaks{1}.x);
for i=1:length(peaks_lr)
    if peaks_lr {i}.x==poix
        peak_first = peaks_lr{i};
        first_i=i;
    end
end
nomult = 0; % Number of multible
max_delta = 0.1;   % 
for i=first_i:length(peaks_lr)
    f = peaks_lr{i}.t / peak_first.t;
    if abs(round(f)-f)<max_delta
        nomult=nomult+1;
    end
end
nomult;

if plot_switch
    figure(savecf)
end

%---x---x---x---x---x---x

% is there is a priori information? 
% if (nargin>2)
%     dominant_ti = mb(h_index) / getsr(ti_profile);
%     result = 0; 
%     return
% end



% ------------ subfunctions ---------------------

% turns a vector (row) upside down
function y=upsidedown(x)
y=[];
for i=length(x):-1:1
    y=[y x(i)];
end
