function f=plus(a,b)
% addition 
% einfachster Fall: Addiere eine konstante Zahl
% sonst: Addiere ein zweites Feld zum Zeitpunkt Null

if isnumeric(b)
    a.werte=a.werte+b;
    f=a;
    return
end

if isobject(a)
    x1=getnrpoints(a);
    y1=getnrfrequencies(a);
    x2=getnrpoints(b);
    y2=getnrfrequencies(b);

    if (x1~=x2) | (y1~=y2) 
        disp('Fields do not fit');
        f=a;
        return;
    end
    f=a;
    f.werte=f.werte+b.werte;
end