% function [result, center_c] = analyse_frequency_profile(fq_profile, peaks)
% 
%   To analyse the time frequency profile of the auditory image
%
%   INPUT VALUES:
%       fq_profile      frequency profile (signal class)
%       peaks           output of peakpicker
%  
%   RETURN VALUE:
%       result      1 means it is a sinusoid spektrum
%       center_c    channel of the highes peak
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/27 17:52:54 $
% $Revision: 1.3 $
function [result, center_c] = analyse_frequency_profile(fq_profile, peaks)

% for debug reasons
plot_switch = 0;

% ------------- some constants ------------------
% left/right decay
% If there are points between peak.x and left.x or right.x which are 
% that factor smaller than peak.y we would set right or left to this points
left_right_realtive_decay_threshold = 0.1;

% Ignore peaks that are 1/mentionable_peak_factor of the highest peak
% assume this is noise
mentionable_peak_factor = 12;       % mentionable peaks factor



%---------------- Code starts here -------------

% Get Information about the peaks
fq_profile_vals = getdata(fq_profile);
if plot_switch
    cf_save = gcf;
    figure(14)
    plot(fq_profile_vals,'r-');
    hold on
    axis auto
end
if length(peaks)<1
    result = 0;
    center_c = 0;
    return
end


nops = length(peaks);   % number of peaks

% peaks.left and peaks.right are are minima between two maxima
% But if there are points closer to the maximum which have a certain value,
% "left" or "right" is set to these points
for i=1:nops
    % Test for left side
    for x=(peaks{i}.x-1):-1:peaks{i}.left.x
        if x<1
            break;
        end
        if fq_profile_vals(x) < (left_right_realtive_decay_threshold*peaks{i}.x)  
            % new left
            peaks{i}.left.x = x;
            peaks{i}.left.y = fq_profile_vals(x);
            break; % break for loop
        end
    
    end
    % test for right side
     for x=(peaks{i}.x+1):peaks{i}.right.x
        if fq_profile_vals(x) < (left_right_realtive_decay_threshold*peaks{i}.x)  
            % new left
            peaks{i}.right.x = x;
            peaks{i}.right.y = fq_profile_vals(x);
            break; % break for loop
        end
    end
end

peaks2 = peaks;  % Peaks2 sortet for highest peak

% calculate the peak weight
% Peak weight is sum of peak/width of peak
% First peak is the highest and has hight_gain=1
height_gain = peaks{1}.y;
for i=1:nops
    peaks{i}.weight = sum(fq_profile_vals(peaks{i}.left.x:peaks{i}.right.x))/(peaks{i}.right.x-peaks{i}.left.x)*(peaks{i}.y/height_gain);
    if plot_switch
        plot(peaks{i}.x, peaks{i}.weight, 'k.');
    end
end
% Sort peaks by decreasing weight
for i=1:(nops-1)
    for j=nops:-1:i
        if (peaks{i}.weight<peaks{j}.weight)
            tmp = peaks{i};
            peaks{i}=peaks{j};
            peaks{j}=tmp;
        end
    end
end

% Test how many "important" peaks
if nops>1
    % Test how many important peaks there are
    % A peak with a weight of at least 1/mentionable_peak_factor of the first and highest peak is important
    % The Idea is to ignor small "noise" peaks
    % Defined now in top of this function mentionable_peak_factor = 10;    % mentionable peaks factor
    noips=1;                         % number of important peaks
    if plot_switch
        plot(peaks{1}.x, peaks{1}.weight, 'r.');
    end
    for i=2:nops
        if (mentionable_peak_factor*peaks{i}.weight)>peaks{1}.weight
            noips=noips+1;
            if plot_switch
                plot(peaks{i}.x, peaks{i}.weight, 'r.');
            end
        end
    end
else
    % there is only one peak, therefore it is important
    noips = 1;          % NumberOfImportantPeakS
end

%--------------- Quantification --------------------------------------
%%%%  First method
% % Now calculate Result.
% if noips==1
%     % assume a single peak to be a sinusoid
%     result=1;
% else
%     % To unstable: 
%     % result = sum(fq_profile_vals(peaks2{1}.left.x:peaks2{1}.right.x)) / sum(fq_profile_vals);
%     if plot_switch
%         plot(peaks2{1}.x, peaks2{1}.y, 'rs');
%     end
%     y_mean = 0;
%     for i=1:noips
%         y_mean = y_mean + peaks{i}.y;
%     end
%     y_mean = y_mean/noips;
%     result = y_mean/peaks2{1}.y;
%     % testing
%     y_sum = 0; 
%     limit = 0.1;
%     for i=1:nops
%         if peaks2{i}.y>(peaks2{1}.y*limit)
%             y_sum = y_sum +peaks2{i}.y;
%             if plot_switch
%                 plot(peaks2{i}.x,peaks2{i}.y,'go');
%             end
%         end
%     end
%     result = peaks2{1}.y / y_sum;
% end

% %% Second method
% %% Calculate the mean peak to valley ratio
% pr =[];
% for i=1:noips
%     prrr =  mean([peaks{i}.left.y peaks{i}.right.y])/peaks{i}.y ;  
%     %prrr = peaks{i}.y/peaks2{1}.y;   % The normalized height 
%     prrr = prrr/log(abs(peaks{i}.left.x-peaks{i}.right.x)+3);
%     pr = [pr prrr];
%     if plot_switch
%         plot([peaks{i}.left.x peaks{i}.right.x],[peaks{i}.left.y peaks{i}.right.y],'og');
%     end
% end
% result = 1 - mean(pr);
% The returned main centre frequency is the frequency with the highest peak
center_c = peaks2{1}.x ;
%     

%% Third method
%% Highest peak / number of neighbourgh channels which are bigger than -xdB
max_attenuation_dB = -10;
atten_fac = 10^(max_attenuation_dB/20);  % attenuation as factor
% Take highest Peak
poi = peaks2{1};
%poi = peaks{1};

fq_profile_vals = fq_profile_vals./poi.y;
maxy =1;
% maxy = poi.y;
pwidth = 0;
x = poi.x;
while (x<=length(fq_profile_vals))&&(fq_profile_vals(x)>maxy*atten_fac)
    pwidth=pwidth+1; % one more channel
    x=x+1;
end
x = poi.x-1;
while (x>=1)&&(fq_profile_vals(x)>maxy*atten_fac)
    pwidth=pwidth+1; % one more channel
    x=x-1;
end
% due to filterbank sinusoid has certain width
% pwidth=pwidth-round(length(fq_profile_vals)/10);
% if pwidth<1
%     pwidth=1;
% end
result =  1-(pwidth/length(fq_profile_vals));


if plot_switch
  figure(cf_save);
end
% % % For debug plot minima
% for i=1:nops
%     plot(peaks{i}.left.x, peaks{i}.left.y, 'ob');
%     plot(peaks{i}.right.x, peaks{i}.right.y, 'xb');
% end

% Problem
% result 
