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
% $Date: 2003/07/09 15:24:56 $
% $Revision: 1.1 $

function [allstrobeprocesses,allthresholds]=gen_channel_integration(nap,options)


options.window_function='linear';



allthresholds=nap;
cfs=getcf(nap);


integrate_octaves_above=options.integrate_octaves_above;
integrate_octaves_below=options.integrate_octaves_below;

window_function=options.window_function;

nr_channels=getnrchannels(nap);


confusion_band_low_start=2;  % -2 octaves
confusion_band_low_stop=2;  % to 2 octaves
confusion_band_low_center=-3;  % at 2 octaves below current cf


% first calculate, which channels must be integrated
for ii=1:nr_channels
    % 	waitbar(ii/nr_channels);
    
    current_fre=cfs(ii);

    
    % calculate, which channels are wanted
    min_selected_frequency=cent2fre(current_fre,-1200*integrate_octaves_below);
    max_selected_frequency=cent2fre(current_fre,+1200*integrate_octaves_above);
    % which channels are these?
    min_channel=round(fre2chan(nap,min_selected_frequency));
    max_channel=round(fre2chan(nap,max_selected_frequency));
    
    % restrict range
    a=[min_channel:max_channel];
    a(find(a<0))=0;
    a(find(a>nr_channels))=0;
    neighboring_channels(ii,1:length(a))=a;
    
    nr_above=max_channel-ii;
    nr_below=ii-min_channel;
    if strcmp(window_function,'linear');
        w_below=linspace(0,1,nr_below+2);
        w_below=w_below(2:end-1);
        w_above=linspace(1,0,nr_above+2);
        w_above=w_above(2:end-1);
    end
    count=1;
    for kk=neighboring_channels(ii,:)
        if count<=nr_below
            if kk>0
                neighboring_channels_weight_factor(ii,count)=w_below(count);
            else
                neighboring_channels_weight_factor(ii,count)=0;
            end
        elseif kk==ii
            neighboring_channels_weight_factor(ii,count)=1;
        else
            if kk>0
                neighboring_channels_weight_factor(ii,count)=w_above(count-nr_below-1);
            else
                neighboring_channels_weight_factor(ii,count)=0;
            end
        end
        count=count+1;
    end
    
    
    
    % the channels, that are in the way!
    % calculate, which channels are wanted
    current_fre=cent2fre(current_fre,1200*confusion_band_low_center);
    min_selected_frequency=cent2fre(current_fre,-1200*confusion_band_low_start);
    max_selected_frequency=cent2fre(current_fre,+1200*confusion_band_low_stop);
    % which channels are these?
    min_channel=round(fre2chan(nap,min_selected_frequency));
    max_channel=round(fre2chan(nap,max_selected_frequency));
    
    % restrict range
    a=[min_channel:max_channel];
    a(find(a<0))=0;
    a(find(a>nr_channels))=0;
    neighboring_channels2(ii,1:length(a))=a;
    current_channel=round(fre2chan(nap,current_fre));
    
    nr_above=max_channel-current_channel;
    nr_below=current_channel-min_channel;
    if strcmp(window_function,'linear');
        w_below=linspace(0,1,nr_below+2);
        w_below=w_below(2:end-1);
        w_above=linspace(1,0,nr_above+2);
        w_above=w_above(2:end-1);
    end
    count=1;
    for kk=neighboring_channels2(ii,:)
        if count<=nr_below
            if kk>0
                neighboring_channels2_weight_factor(ii,count)=w_below(count);
            else
                neighboring_channels2_weight_factor(ii,count)=0;
            end
        elseif kk==ii
            neighboring_channels2_weight_factor(ii,count)=1;
        else
            if kk>0
                if count-nr_below-1 >0
                    neighboring_channels2_weight_factor(ii,count)=w_above(count-nr_below-1);
                else
                    neighboring_channels2_weight_factor(ii,count)=w_above(count-nr_below);
                end
            else
                neighboring_channels2_weight_factor(ii,count)=0;
            end
        end
        count=count+1;
    end
    grafix=1;
    
    if grafix
        figure(423543)
%         clf
        hold on
        cols=['b','k','r','g','m','y','c'];
        col=cols(mod(ii,7)+1);
        xse=neighboring_channels(ii,:);
        yse=neighboring_channels_weight_factor(ii,:);
        yse=yse(find(xse>0));
        xse=xse(find(xse>0));
        plot(xse,yse,'.-','Color',col);
        
        xse=neighboring_channels2(ii,:);
        yse=neighboring_channels2_weight_factor(ii,:);
        yse=yse(find(xse>0));
        xse=xse(find(xse>0));
        plot(xse,yse,'.-','Color',col);
        p=0;
    end
end
    


% translate all napchannels in their leaky integrator lowpassed versions:
short_time_constant=options.short_time_constant;
% long_time_constant=options.long_time_constant;
lowpass_cutoff_frequency=1/(2.*pi.*short_time_constant);

storenap=nap;
for jj=1:nr_channels
    cur_chan=getsinglechannel(nap,jj);
    cur_chan=leakyintegrator(cur_chan,lowpass_cutoff_frequency,1);
    nap=setsinglechannel(nap,jj,cur_chan);
end


% now do the interchannel summarization

current_threshold=zeros(nr_channels,1);
sr=getsr(nap);
napvals=getvalues(nap);
tresholdvals=zeros(size(napvals));
newnapvals=zeros(size(napvals));
% accumvals=zeros(size(napvals));
% dgvals=zeros(size(napvals));


nr_dots=length(napvals);
times_per_ms=round(sr*0.005); % how often the bar should be updated
counter=ones(nr_channels,1);

% values for the fast leaky integration
short_leaky_b=exp(-1/(sr.*short_time_constant));
short_leaky_gain=1./(1-short_leaky_b);
leaky_yn1=zeros(1,nr_channels);
last_threshold=zeros(1,nr_channels);

waithand=waitbar(0,'generating Delta Gamma Strobes');

% now the big loop over the time
for ii=2:nr_dots-1
    current_time=ii/sr;
    if mod(ii,times_per_ms)==0
        waitbar(ii/nr_dots);
    end
    
    for jj=1:nr_channels
        %     for jj=25
        % the current value of each channel is the weighted sum of all
        % low_pass filterted channels around it
        current_val=0;
        count=0;
        for chn=neighboring_channels(jj,:);
            if chn>0
                count=count+1;
                weight=neighboring_channels_weight_factor(jj,count);
                current_val=current_val+weight*napvals(chn,ii);
            end
        end
        % neighbouring channels
        count2=0;
        for chn=neighboring_channels2(jj,:);
            if chn>0
                count2=count2+1;
                weight=neighboring_channels2_weight_factor(jj,count2);
                current_val=current_val+weight*napvals(chn,ii);
            end
        end
        current_val_weighted_sum=current_val/(count+count2);
        current_val=current_val_weighted_sum;
        
        newnapvals(jj,ii)=current_val;
    end
end
close(waithand);
% copy the values to the new nap
newnap=setvalues(nap,newnapvals);
% 

% and now do the standard strobe finding
strobeoptions.strobe_criterion='parabola';
strobeoptions.strobe_decay_time=0.02;
strobeoptions.parabel_heigth=1.4;
strobeoptions.parabel_width_in_cycles=4.5;
[allstrobeprocesses,allthresholds]=gen_sf2003(newnap,strobeoptions);

% now find the nearest local maximum in the real nap to the strobe points

for jj=1:nr_channels
    allstr=allstrobeprocesses{jj}.strobes;
    nr_str=length(allstr);
    cur_chan=getsinglechannel(nap,jj);
    for ii=1:nr_str
        [maxpos,minpos,maxs,mins]=getminmax(cur_chan);
        [maxpos1,max1]=getmaximumleftof(allstr(ii),maxpos,minpos,maxs,mins);
        [maxpos2,max2]=getmaximumrightof(allstr(ii),maxpos,minpos,maxs,mins);
        found_max=0;
        if ~isempty(max1)
            allstrobeprocesses{jj}.strobes(ii)=maxpos1;
            allstrobeprocesses{jj}.strobe_vals(ii)=max1;
        end
        if ~isempty(max2)
            if max2>allstrobeprocesses{jj}.strobes(ii);
                allstrobeprocesses{jj}.strobes(ii)=maxpos2;
                allstrobeprocesses{jj}.strobe_vals(ii)=max2;
            end
        end
    end
end


