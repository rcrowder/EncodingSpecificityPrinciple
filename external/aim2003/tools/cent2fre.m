

function hz=cent2fre(f0,cent)
% usage: hz=cent2fre(sp,cent,oct)
% returns the frequency of a cent-note, or this frequency in another octave
% base is "A" at 27.5 Hz

hz=f0*power(2,((cent)/1200));
