% function [result, dominant_ti] =  analyse_timeinterval_profiletunedDP(ti_profile, peaks, a_priori, fqp_fq, apTIP)
%
%   To analyse the time interval profile of the auditory image
%
%   INPUT VALUES:
%           ti_profile      time-interval profile (signal class)
%           peaks           output of peakpicker 
%           a_priori        a_priori information - not used
%           fqp_fq          result of frequency profile - not used 
%           apTIP           a_priori information (opt) used 
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
function [result, dominant_ti] = analyse_timeinterval_profiletunedDP(ti_profile, peaks, a_priori, fqp_fq, apTIP)

% for debug resons
plot_switch = 0;

intsum = ti_profile;   
intsum_vals = getdata(intsum);
if plot_switch
    %-- debug plot ---
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

% Check if there are any peaks
if length(peaks)<=1
    % If there is no peak than we just plot the function
    result = 0;
    dominant_ti = 0;
    return;
end
   
% ++++++++++++ Part one: Peak finding ++++++++++
% not working too well -> a priori peak picking is used
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

peak_oi = peaks_lr{1};
peak_oi_left = peak_oi;

% This code is used when an a priori peak information
% was given

if apTIP>0
    % take nearest
    dist=+inf;
    indexOI = 1;
    for p=1:length(peaks_lr)
        d=abs(apTIP-1/peaks_lr{p}.t);
        if d<dist
            indexOI=p;     
            dist=d;
        end
    end %p
    peak_oi = peaks_lr{indexOI};
    
    if (indexOI>1)
        peak_oi_left = peaks_lr{indexOI-1};
    else
        peak_oi_left.x=peak_oi.left.x;
        peak_oi_left.y=peak_oi.left.y;
        peak_oi_left.t=peak_oi.left.t;
    end
  
    % if there is no peak near a 1 oktave range, take minimum of
    % peak_oi as peak_oi_left (for very high carrier
    if ((1/peak_oi_left.t)>(2*(1/peak_oi.t)))
        peak_oi_left.x=peak_oi.left.x;
        peak_oi_left.y=peak_oi.left.y;
    end
end
% ++++++++++++ Part two: Quantification ++++++++++

% Ration between peak of envelope and envelope
result = 1 -  intsum_vals(peak_oi_left.x)/intsum_vals(peak_oi.x);
dominant_ti = peak_oi.x /getsr(ti_profile);

% For sinusoids
if result<0
    result=0;
end

if plot_switch
     %-- debug plot ---
    plot(tip_x(peak_oi_left.x), peak_oi_left.y, 'm^');
    plot(tip_x(peak_oi.x), peak_oi.y, 'ro');
    figure(savecf)
end


% ------------ subfunctions ---------------------

% turns a vector (row) upside down
function y=upsidedown(x)
y=[];
for i=length(x):-1:1
    y=[y x(i)];
end

% calculate peak weight
function w=getpeakweight(pk)
%pkheight= mean([pk.left.y pk.right.y])/pk.y;
pkheight=pk.y-mean([pk.left.y pk.right.y]);
pkwidth=abs(pk.left.x-pk.right.x)^3;
w=pkheight/pkwidth;
