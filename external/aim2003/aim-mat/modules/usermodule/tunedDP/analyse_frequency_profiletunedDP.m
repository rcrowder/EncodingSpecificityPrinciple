% function [result, center_c] = analyse_frequency_profiletunedDP(fq_profile, peaks, apFQP)
% 
%   To analyse the time frequency profile of the auditory image
%   Returns value between 0 and 1 discribing the strength of the spectral
%   pitch. Used for quantitative analysis of ramped and damped sinusoids
%   and for sinusoidally amplitude modulated sinusoids
%
%
%   INPUT VALUES:
%       fq_profile      frequency profile (signal class)
%       peaks           output of peakpicker
%       apFQP           a priori information where to find the peak
%  
%   RETURN VALUE:
%       result      0..1; 
%       center_c    channel of the highes peak
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/27 17:52:54 $
% $Revision: 1.3 $
function [result, center_c] = analyse_frequency_profiletunedDP(fq_profile, peaks, apFQP)

% for debug reasons -
plot_switch = 0;


% ------------- some constants ------------------


%---------------- Code starts here -------------

fq_profile_vals = getdata(fq_profile);
if plot_switch
    cf_save = gcf;
    figure(13)
    cla;
    plot(fq_profile_vals,'r-');
    hold on
    axis auto
end
% stop if there are no peaks, e.g. in the first frames
if length(peaks)<1
    result = 0;
    center_c = 0;
    return
end

% Peak of interest is highest peak of the profile
peaks_oi = peaks{1};

if apFQP>0
    % no peak finding - peak is given a priori
    % take nearest peak to a apriori frequency
    dist=+inf;
    indexOI = 1;
    for p=1:length(peaks)
        d=abs(apFQP-peaks{p}.t);
        if d<dist
            indexOI=p;     
            dist=d;
        end
    end %p
    peak_oi = peaks{indexOI};
end


% %%  Method --- works with SAM sounds
% %%  Highest peak / number of neighbourgh channels which are bigger than -xdB
% max_attenuation_dB = -6;
% atten_fac = 10^(max_attenuation_dB/20);  % attenuation as factor
% % Take highest Peak
% %poi = peaks2{1};
% %poi = peaks{1};
% poi = peaks_oi;
% 
% fq_profile_vals = fq_profile_vals./poi.y;
% maxy =1;
% % maxy = poi.y;
% pwidth = 0;
% x = poi.x;
% while (x<=length(fq_profile_vals))&&(fq_profile_vals(x)>maxy*atten_fac)
%     pwidth=pwidth+1; % one more channel
%     x=x+1;
% end
% if plot_switch
%   line([x x],[0 peaks_oi.y]);
% end
% 
% x = poi.x-1;
% while (x>=1)&&(fq_profile_vals(x)>maxy*atten_fac)
%     pwidth=pwidth+1; % one more channel
%     x=x-1;
% end
% if plot_switch
%   line([x x],[0 peaks_oi.y]);
% end
% result_width =  1-(pwidth/length(fq_profile_vals));
% 

% ----------------------------
% Method developed with Roy 13/06
% Calculate the mean of the Channels in a 20 to 80 percent
% range left of the main peak
startx=floor(0.2*peaks_oi.x);
stopx=floor(0.8*peaks_oi.x);
result = 1 - mean(fq_profile_vals(startx:stopx))/fq_profile_vals(peaks_oi.x);

center_c = peaks_oi.x;

if plot_switch
  plot(peaks_oi.x,peaks_oi.y,'r.');
  plot(startx,fq_profile_vals(startx),'bx');
  plot(stopx,fq_profile_vals(stopx),'bx');
  figure(cf_save);
end
% % % For debug plot minima
% for i=1:nops
%     plot(peaks{i}.left.x, peaks{i}.left.y, 'ob');
%     plot(peaks{i}.right.x, peaks{i}.right.y, 'xb');
% end

