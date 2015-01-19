% method of class @fsignal (derived from @signal)
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/06/18 12:50:15 $
% $Revision: 1.6 $

function plot(fsig,border,stil)

if nargin==2
    if ischar(border)
        stil=border;
    else 
        stil='b-';
    end
end

maxfre=fsig.max_fre;    % die höchste Frequenz im Signal
df=fsig.df;         % der Frequenzabstand
nr=getnrpoints(fsig);

werte=getdata(fsig);
mi=min(werte);
ma=max(werte);
if mi==ma
    ma=mi+1;
end


% minx=1;
% maxx=nr-1;

if nargin <2
    stil='b-';
    border=[df maxfre mi ma];
else
    if ischar(border)
        border=[df maxfre mi ma];
    else
        nr=size(border,2);
        if nr==2    % wenn nur die x-Werte angegeben werden
            border=[border(1) border(2) mi ma];
        end
    end
end

minf=border(1);
maxf=border(2);

minx=freq2bin(fsig,minf);
maxx=freq2bin(fsig,maxf);


plotw=werte(2:end);  % die Null vorne wird abgeschnitten!


plotlin=1;

if plotlin==0
    semilogx(plotw,stil);
    axis([minx maxx border(3) border(4)]);
    xt=get(gca,'XTick');
    xt=distributelogarithmic(minx,maxx,8);
    set(gca,'XTick',xt);
    xti=bin2freq(fsig,xt);
    xti=fround(xti,1);
    set(gca,'XTickLabel',xti);
else
    % to make the frequencies right, double every point...
    plotwnew=[zeros(size(plotw));zeros(size(plotw))];
    for i=1:length(plotw);
        plotwnew((i-1)*2+1)=plotw(i);
        plotwnew((i-1)*2+2)=plotw(i);
    end        
    plot(plotwnew,stil);
    axis([minx maxx border(3) border(4)]);
%     xt=get(gca,'XTick');
%     xt=distributelogarithmic(minx,maxx,8);
%     set(gca,'XTick',xt);
%     xti=bin2freq(fsig,xt);
%     xti=fround(xti,1);
%     set(gca,'XTickLabel',xti);
    
end


% xstep=(log(maxx)-log(minx))/10;
% tix=exp(log(minx):xstep:log(maxx));

% fstep=(log(maxf)-log(minf))/10;
% ti=(log(minf):fstep:log(maxf));
% ti=exp(ti)/1000;
% ti=fround(ti,1);
%     semilogx(fsig.werte,stil);


xlabel(getunit_x(fsig));
ylabel(getunit_y(fsig));
title(getname(fsig),'Interpreter','none');
