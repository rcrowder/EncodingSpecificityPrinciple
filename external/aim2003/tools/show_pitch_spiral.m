% tool
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:45 $
% $Revision: 1.3 $

function show_pitch_spiral(frame)

cls;
s=getallnotes;

min_fre=abs(1000/frame.time_minus);
max_fre=abs(1000/frame.time_plus);
max_fre=2000;

sp=spiral(min_fre,max_fre);
sp=adddots(sp,s);

frame.search_type='best_n';
frame.nr_search_maxima=50;

[pitches,s]=findpitch(frame);

sp=adddots(pitches,s);
plot(sp);
set(gca,'Position',[0 0.2 1 0.8]);
% 
% % and the sum
rect=[-0.04 0 1 0.2];
mysubplot(1,1,1,rect,1);

nr_bins=24;
tones=pitch_radar(sp,nr_bins);  % here it is calculated
h=plot(tones,'.-');
set(gca,'yticklabel','')
axis([0.5 nr_bins+0.5 0 3]); 
tix=1:nr_bins/12:nr_bins;
ti=['a ';'a#';'h ';'c ';'c#';'d ';'d#';'e ';'f ';'f#';'g ';'g#'];
set(gca,'XTick',tix);
set(gca,'XTickLabel',ti);


return;