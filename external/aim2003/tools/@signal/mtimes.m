% method of class @signal
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/07/28 14:20:21 $
% $Revision: 1.4 $

function sig=mtimes(a,b)
% multiplikation mit *
% einfachster Fall: Multipliziere mit konstanter Zahl
% sonst: Multipliziere ein zweites Signal zum Zeitpunkt Null
% a muss ein Signal sein!!!

if isnumeric(b) && isobject(a)
    if size(b)==1
        a.werte=a.werte*b;
        sig=a;
    else
        d1=getnrpoints(a);
        d2=max(size(b));
        if d1>d2 % nimm den kleineren WErt
            dauer=bin2time(a,d2);
        else
            dauer=bin2time(a,d1);
        end
%         if dauer>getmaximumtime(a)
%             dauer=getmaximumtime(a);
%         end
        sig=mult(a,b,0,dauer);
    end
    
elseif isnumeric(a) && isobject(b)
    if size(a)==1
        b.werte=b.werte*a;
        sig=b;
    else
        d1=getnrpoints(b);
        d2=max(size(a));
        if d1>d2 % nimm den kleineren WErt
            dauer=bin2time(b,d2);
        else
            dauer=bin2time(b,d1);
        end            
        sig=mult(b,a,0,dauer);
    end
elseif isobject(a) && isobject(b)
    dauer=getlength(a);
    start=getminimumtime(a);
    sig=mult(a,b,start,dauer);
end
