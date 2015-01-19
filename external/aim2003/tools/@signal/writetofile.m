% method of class @
%	function writetofile(sig,name) 
%   INPUT VALUES:
%  		sig: signal to save
%		name: filename
%   RETURN VALUE:
%		none
%
% (c) 2003, University of Cambridge, Medical Research Council 
% Stefan Bleeck (stefan@bleeck.de)
% http://www.mrc-cbu.cam.ac.uk/cnbh/aimmanual
% $Date: 2003/01/17 16:57:43 $
% $Revision: 1.3 $

function writetofile(sig,name)
%usage: writetofile(sig,name)
%DRRSmith 28/05/02
%readsounddata=getdata(sig);
%fid=fopen(name,'wb');
%fwrite(fid,readsounddata,'int16');
%fclose(fid);

if nargin<3

readsounddata=getdata(sig);
if (isunix==1)
   fid=fopen(name,'wb','b'); %bigendian
   fwrite(fid,readsounddata,'int16');
   fclose(fid);
else
   fid=fopen(name,'wb','l'); %littleendian
   fwrite(fid,readsounddata,'int16');
   fclose(fid);
end

% readsounddata=getdata(sig);
% fid=fopen(name,'wb',endian);
% fwrite(fid,readsounddata,'int16');
% fclose(fid);
