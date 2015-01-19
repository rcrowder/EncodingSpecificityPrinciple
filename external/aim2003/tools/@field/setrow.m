function f=setrow(field,x,sig)
% setze die Zeile (links rechts) mit den Werten aus Signal
% erster Parameter in einem Matlab Array immer die Frequenz (oben-unten!!!)


if isobject(sig)
    nr_signal=getnrpoints(sig);
else
    nr_signal=size(sig,2);
end

nr_field=getnrpoints(field);
if nr_signal ~= nr_field
    disp('different number of points in field::setrow');
    return
end

f=field;

if isobject(sig)
    wer=getdata(sig);
else
    wer=sig;
end

f.werte(x,1:nr_field)=wer(1:nr_signal);
  