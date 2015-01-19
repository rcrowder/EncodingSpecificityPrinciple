function display(f)

disp('Object of Class field');
% disp(sprintf('Name: %s',fsig.name));
disp(sprintf('Time: %5.2f sec',getduration(f)));
disp(sprintf('maximum frequency %5.2f ',maxfre(f)));
disp(sprintf('Sample rate: %5.2f Hz',f.samplerate));
disp(sprintf('Time steps: %d',getnrt(f)));
disp(sprintf('Frequency steps: %d',getnrf(f)));

