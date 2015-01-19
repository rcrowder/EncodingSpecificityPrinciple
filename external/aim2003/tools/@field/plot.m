function plot(f,modus)

if nargin<2
    modus='linear';
end

% maxfre=maxfre(f);
% nrf=getnrf(f);
% freqs=linspace(0,maxfre,nrf);
% 
% nrt=getnrt(f);
% sr=getsr(f);
% maxt=sr*nrt;
% times=linspace(0,maxt,nrt);
% startt=getoffset(f);
% times=times-startt;
% 
% vals=getvalues(f);
% imagesc(times,freqs,20*log10(vals));
% axis xy
% colormap(jet)


surf(f.werte);
% view(0,90);
shading interp

maxx=getnrt(f);
maxy=getnrf(f);
minx=0;
miny=0;

xstep=(maxx-minx)/10;
tix=minx:xstep:maxx;
ti=([minx:xstep:maxx]);
ti=bin2time(ti,f.samplerate);
ti=round((ti+f.offset)*10000)/10;
set(gca,'XTick',tix);
set(gca,'XTickLabel',ti);

if strcmp(modus,'linear')
    ystep=(maxy-miny)/10;
    tiy=miny:ystep:maxy;
    ti=([miny:ystep:maxy]);
    ti=ti/maxy*f.maxfre;
    ti=round(ti*10)/10000;
    set(gca,'YTick',tiy);
    set(gca,'YTickLabel',ti);
elseif strcmp(modus,'log')
    set(gca,'yscale','log')

    max_fre=maxfre(f);
    nr_ticks=6;
    yticks=distributelogarithmic(maxy/power(2,nr_ticks-1),maxy,nr_ticks);
    set(gca,'YTick',yticks);

    ytickl=distributelogarithmic(max_fre/power(2,nr_ticks-1),max_fre,nr_ticks);
    ti=[];
    for i=1:nr_ticks
        sadf=sprintf('%3.2f',ytickl(i)/1000);
        ti=[ti ; sadf(1:4)];
    end
    set(gca,'YTickLabel',ti);
end
xlabel('time (ms)');
ylabel('Frequency (kHz)');

axis([1 maxx 1 maxy]);



