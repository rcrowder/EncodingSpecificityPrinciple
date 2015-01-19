function f=mtimes(f,b)
% multiplikation mit *
% einfachster Fall: Multipliziere mit konstanter Zahl
% sonst: Multipliziere ein zweites Signal zum Zeitpunkt Null

if isnumeric(b)
    if size(b)==1
        f.werte=f.werte*b;
    else
        disp('field::mtimes: too many parameters in multiplication')
    end
    return
end

disp('field::mtimes: multiplication with field not implemented yet')
