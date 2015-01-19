function f=setcolumn(field,x,sig)
% setze die Spalte (hoch-runter) mit den Werten aus Signal
% erster Parameter in einem Matlab Array immer die Frequenz (oben-unten!!!)

if isobject(sig)
    nr_signal=getnrpoints(sig);
else
    nr_signal=size(sig,1);
end

nr_field=getnrfrequencies(field);
if nr_signal ~= nr_field
    disp('different number of points in field::setcolum');
    return
end

if x>field.numbert; % das Feld kann sich bei der Aktion vergrößern
    field.numbert=x;
end
f=field;
if isobject(sig)
    wer=getdata(sig);
else
    wer=sig;
end
f.werte(1:nr_field,x)=wer(1:nr_signal);
    
