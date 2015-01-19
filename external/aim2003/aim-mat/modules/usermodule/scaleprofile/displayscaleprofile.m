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
% $Revision: 1.2 $

function displaydualprofile(sai,options,frame_number)

%setup the scale bar to alter to range of the colour map
%converts the exponential scale on the scale bar to a value 0<x<1
slider_value = slidereditcontrol_get_value(options.handles.slideredit_scale);
options.max_value=(-log10(slider_value)+3.001)/6;
% disp('the max value of the colour map is:');
% disp(options.max_value);
%read in the mellin information to matricies
current_frame=sai{frame_number};
matrix_of_current_frame=getvalues(current_frame);
matrix_of_current_frame = matrix_of_current_frame / max(current_frame);
%set the range of values for the axes
harm_range=[0,size(matrix_of_current_frame,1)];
size_range=[0,size(matrix_of_current_frame,2)];

%set the resolution of the axes
coef_step=(harm_range(1,2)-harm_range(1,1))/10;
%h_step=(h_range(1,2)-h_range(1,1))/10;
size_step=1;

%sets the axis divisions
harm_axis = [harm_range(1,1):coef_step:harm_range(1,2)];
size_axis = [size_range(1,1):size_step:size_range(1,2)];

%this section sets up the colormap to be the correct gray scale version that we want 
colormap_name=gray(128);  % maah: was = gray(128)
size_colormap=size(colormap_name);
%disp(size_colormap);
for ii=1:size_colormap(1);
   rich_map(ii,:)=colormap_name((129-ii),:);
end;
colormap(rich_map);


%now we generate the image matlab automatically scales the colours
%note that we take the magnitude of the components
%we reset the the colourmap, scaling it's maximum to 1
%matrix_of_current_frame = matrix_of_current_frame'; %removed the transpose
%in the display function and put it into the generating function 
scale_image = image(size_range, harm_axis, matrix_of_current_frame,'CDataMapping','scaled');

set(gca,'CLimMode','manual');
set(gca,'CLim',[0 options.max_value]);

%now we scale the image so that it fills the display area
limitx=ceil(max(size_axis));
limity=ceil(max(harm_axis));
set(gca,'XLim',[0 limitx]);
set(gca,'YLim',[0 limity]);

xlabs=get(gca,'xticklabel');
xnlab=bin2time(current_frame,str2num(xlabs));
xnlab=fround(xnlab,1);
set(gca,'xticklabel',xnlab);

set(gca,'YTick', harm_axis);
set(gca,'YTickLabel', harm_axis,'FontSize',8,'YAxisLocation','right');

%flip the y axis
set(gca,'YDir','normal')

%and put on the labels
xlabel('harmonic relation');
ylabel('size scale variable');

% 
% % Test if the frame_number is available
% if length(sai)<frame_number
%     % no its not
%     return
% end
% 
% 
% 
% spro=sai{frame_number}.scale_profile;
% cla
% maxf=max(spro);
% 
% plot(spro/maxf);
% 
% set(gca,'YAxisLocation','right');
% sr=getsr(spro);
% xlabs=get(gca,'xticklabel');
% xnlab=str2num(xlabs)/1000;
% xnlab=fround(xnlab,1);
% set(gca,'xticklabel',xnlab);