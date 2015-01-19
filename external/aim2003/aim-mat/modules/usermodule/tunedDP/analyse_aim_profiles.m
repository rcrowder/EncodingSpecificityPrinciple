% function  result = analyse_aim_profiles(ti_profile, fq_profile, peaks_tip, peaks_fqp, channel_center_fq)
%
%   To analyse the time interval and frequency profile of the auditory image
%
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% Christoph Lindner
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/05/16 13:08:54 $
% $Revision: 1.1 $
function result = analyse_aim_profiles(ti_profile, fq_profile, peaks_tip, peaks_fqp, channel_center_fq )


[fqp_result, fqp_fq] = analyse_frequency_profile(fq_profile, peaks_fqp);
if fqp_result==1
    [tip_result, dominant_ti] = analyse_timeinterval_profile(ti_profile, peaks_tip, fqp_result, fqp_fq); 
else
    [tip_result, dominant_ti] = analyse_timeinterval_profile(ti_profile, peaks_tip);
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
