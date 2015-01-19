% tool
% 
%   INPUT VALUES:
%  
%   RETURN VALUE:
%
% 
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:44 $
% $Revision: 1.3 $

function new=DSAMAddOutputFile(in,filename,endian,samplerate)



% 
% DataFile_Out	<	outputDflt.par
% }
% ##----- DataFile_Out         [0]     (outputDflt.par) -----##
% FILENAME.DataFile_Out.13 	"c:\temp\default\model.aif"	Stimulus file name.
% WORDSIZE.DataFile_Out.13 	2         	Default word size for sound data (1,2 or 4 bytes)
% ENDIAN_MODE.DataFile_Out.13	"BIG"     	Default (for format), 'little' or 'big'-endian.
% CHANNELS.DataFile_Out.13 	1         	No. of Channels: for raw binary and ASCII files.)
% NORM_MODE.DataFile_Out.13	-1        	Normalisation factor for writing (0 or -ve: automatic).
% SAMPLERATE.DataFile_Out.13	20000     	Default sample rate: for raw binary and ASCII files (Hz).
% DURATION.DataFile_Out.13 	-1        	Duration of read signal: -ve = unlimited (s)
% STARTTIME.DataFile_Out.13	0         	Start time (offset) for signal (s).
% GAIN.DataFile_Out.13     	0         	Relative signal gain (dB).


new=in;

nr=size(in,2);
linenr=0;
for i=1:nr
    is_header=strfind(in{i},'}');
    linenr=i;
    if is_header    %stop, when out of header
        break;
    end
end
% now we know, where the header ends. We substitute { by all of our stuff:

% first: shift all lines 12 lines to the bottom:
new{linenr}=' ';    % get rid of the }
for i=nr+11:-1:linenr
    new{i}=new{i-11};
end

% then insert all relevant lines
str1=sprintf('DataFile_Out	<	outputDflt.par');
str2='}';
str3=sprintf('FILENAME.DataFile_Out.13 	"%s"	Stimulus file name.',filename);
str4=sprintf('WORDSIZE.DataFile_Out.13 	%d         	Default word size for sound data (1,2 or 4 bytes)',2);
switch endian
case 'b'
    str5=sprintf('ENDIAN_MODE.DataFile_Out.13	"BIG"');
case 'l'
    str5=sprintf('ENDIAN_MODE.DataFile_Out.13	"LITTLE"');
end
str6=sprintf('CHANNELS.DataFile_Out.13 	%d         	No. of Channels: for raw binary and ASCII files.)',1);
str7=sprintf('NORM_MODE.DataFile_Out.13	%f        	Normalisation factor for writing (0 or -ve: automatic).',-1);
str8=sprintf('SAMPLERATE.DataFile_Out.13	%f     	Default sample rate: for raw binary and ASCII files (Hz).',samplerate);
str9=sprintf('DURATION.DataFile_Out.13 	%f        	Duration of read signal: -ve = unlimited (s)',-1);
str10=sprintf('STARTTIME.DataFile_Out.13	%f         	Start time (offset) for signal (s).',0);
str11=sprintf('GAIN.DataFile_Out.13     	%f         	Relative signal gain (dB).',0);

new{linenr}=str1;
new{linenr+1}=str2;
new{linenr+2}=str3;
new{linenr+3}=str4;
new{linenr+4}=str5;
new{linenr+5}=str6;
new{linenr+6}=str7;
new{linenr+7}=str8;
new{linenr+8}=str9;
new{linenr+9}=str10;
new{linenr+10}=str11;

