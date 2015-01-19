% function  result = analyse_aim_profilestunedDP(ti_profile, fq_profile, peaks_tip, peaks_fqp, channel_center_fq, apTIP, apFQP )
%
%   To analyse the time interval and frequency profile of the auditory image
%
%   INPUT VALUES:
%              ti_profile       Time interval Profile (class signal)
%              fq_profile       Frequency Profile (class signal)
%              peaks_tip        Peaks of time interval Profile (struct, result from peakpicker)
%              peaks_fqp        Peaks of frequency profile (struct, result from peakpicker)
%              channel_center_fq  centre frequ. of channels - for debug plots
%              apTIP            a priori info in Hz where tro finde residue peak
%              apFQP            a priori info where to find spectral main peak 
%              if last two parameters are -1 --> peak is detected automatically        
%
%   RETURN VALUE: 
%              struct: 
%               tip:     residue pitch strength value (report: RPS)
%               fqp:     spectral pitch strength (report: SPS)
%               dti:     reasidue peak       
%               dfq:     main spectral peak  
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/27 17:52:54 $
% $Revision: 1.2 $
function result = analyse_aim_profilestunedDP(ti_profile, fq_profile, peaks_tip, peaks_fqp, channel_center_fq, apTIP, apFQP )

%   Complicated structure because meant to compare information coming from
%   both profile-analyses functions.
%   If very much like a sinusoid in spectral profile, than no TIP analysis, for example
%   !!! this is not implemented, yet !!!

[fqp_result, fqp_fq] = analyse_frequency_profiletunedDP(fq_profile, peaks_fqp, apFQP);
if fqp_result==1
    [tip_result, dominant_ti] = analyse_timeinterval_profiletunedDP(ti_profile, peaks_tip, fqp_result, fqp_fq, apTIP); 
else
    [tip_result, dominant_ti] = analyse_timeinterval_profiletunedDP(ti_profile, peaks_tip, fqp_result, fqp_fq, apTIP);
end

% return results
if fqp_fq==0
    cfq = 0;
else
    cfq =  channel_center_fq(fqp_fq);
end
if dominant_ti==0
    dti = 0;
else
    dti = 1/dominant_ti;
end
result.tip = tip_result;
result.dti = dti;
result.fqp = fqp_result;
result.dfq = cfq;



%--------------- subfunction -----------------------
