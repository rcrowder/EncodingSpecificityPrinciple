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
% $Date: 2003/07/17 10:58:38 $
% $Revision: 1.1 $

function [ret_allstrobeprocesses,ret_allthresholds]=gen_grouping(nap,options)

grafix=0;


% first produce the standart strobes
% parameters relevant for the calculation of this module
options.strobe_criterion='parabola'; % can be 'parabola', 'bunt','adaptive'
options.strobe_decay_time=0.02;
options.parabel_heigth=1.3;
options.parabel_width_in_cycles=3.5;
[allstrobeprocesses,ret_allthresholds]=gen_sf2003(nap,options);


% now look for two sources:
nr_possible_sources=2;
strobe_lasting_time=0.002;
nr_channels=getnrchannels(nap);
inter_maxtrix=zeros(nr_channels,nr_channels);
cross_strobes_channels=zeros(nr_channels,100,nr_channels);
cross_strobes_values=zeros(nr_channels,100,nr_channels);
for ii=1:nr_channels-1
    strobes_ii=allstrobeprocesses{ii}.strobes;
    if length(strobes_ii)==0
        continue
    end
    for kk=1:length(strobes_ii)
        count=1;
        stobe_time_ii=strobes_ii(kk);
        for jj=1:nr_channels
            strobes_jj=allstrobeprocesses{jj}.strobes;
            if length(strobes_jj)==0
                continue
            end
            for ll=1:length(strobes_jj)
                stobe_time_jj=strobes_jj(ll);
                if stobe_time_ii >=stobe_time_jj-strobe_lasting_time && stobe_time_ii <= stobe_time_jj
                    cross_strobes_channels(ii,kk,count)=jj;
                    cross_strobes_values(ii,kk,count)=allstrobeprocesses{jj}.strobe_vals(ll);
                    count=count+1;
                    break;  % not more then one strobe can contribute
                end
            end
        end
    end
end

cross_strobes=zeros(nr_channels,100,nr_possible_sources);
for ii=1:nr_channels
    strobes_ii=allstrobeprocesses{ii}.strobes;
    for jj=1:length(strobes_ii)
        crs=cross_strobes_channels(ii,jj,:);
        crv=cross_strobes_values(ii,jj,:);
        crv=crv(find(crs>0));
        crs=crs(find(crs>0));
        whistres=zeros(nr_possible_sources,1);
        steps=nr_channels/nr_possible_sources;
        for kk=steps:steps:nr_channels+steps
            for ll=1:length(crs)
                if crs(ll)>kk-steps && crs(ll)<=kk
                    weight=crv(ll);
                    whistres(round(kk/steps))=whistres(round(kk/steps))+weight;
                end
            end
        end
%         histres=hist(crs,[1:5:50]);
        cross_strobes(ii,jj,:)=whistres;

        if grafix
            figure(2143)
            bar(1:5:50,whistres)
            set(gca,'YLim',[0 500]);
            set(gca,'XLim',[0 51]);
        end
    end
end


% collect strobes from same source

ret_allstrobeprocesses.grouped=true;
ret_allstrobeprocesses.original=allstrobeprocesses;

for ii=1:nr_channels
    strobes_ii=allstrobeprocesses{ii}.strobes;
    strobe_number=length(strobes_ii);
    single_channel=getsinglechannel(nap,ii);   
%     new_processes{ii}.cross_channel=zeros(100,nr_possible_sources);
%     new_processes{ii}.cross_channel_numbers=zeros(100,nr_channels);
    if strobe_number==0
        s=0;
        new_processes{ii}.strobe_times=[];
        new_processes{ii}.strobe_vals=[];
    end
    for jj=1:strobe_number;
        strobe_time=allstrobeprocesses{ii}.strobes(jj);
        strobe_val=gettimevalue(single_channel,strobe_time);
        new_processes{ii}.strobe_times(jj)=strobe_time;
        new_processes{ii}.strobe_vals(jj)=strobe_val;
        new_processes{ii}.cross_channel_number(jj,:)=cross_strobes_channels(ii,jj,:);
        new_processes{ii}.cross_channel_value(jj,:)=cross_strobes_values(ii,jj,:);
        for kk=1:nr_possible_sources
            new_processes{ii}.source_cross_channel_value(jj,kk)=cross_strobes(ii,jj,kk);
        end
    end
end

ret_allstrobeprocesses.cross_strobes=new_processes;





return

