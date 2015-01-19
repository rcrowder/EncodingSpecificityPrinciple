function edit(sig)
% opens the signal in CoolEdit (if installed)
name='temp_signal_in_edit';

filename=get_new_filename(name,'wav');

savewave(sig,filename);
try
    winopen(filename);
catch
    error('sorry, no wave editor installed');
end

