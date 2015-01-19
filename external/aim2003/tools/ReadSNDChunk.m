%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			Read16Bits.m
% Purpose:		Read two 8 bit bytes and combine according to the machine's
%				endian architecture.
% Comments:	
% Author:		L. P. O'Mard
% Revised by:
% Created:
% Updated:
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function data=Read16Bits(fid, littleEndian)

first = fread(fid, 1, 'uint8');
second = fread(fid, 1, 'uint8');
if (littleEndian ~= 0)
	data = bitshift(first, 8) + second;
else
	data = bitshift(second, 8) + first;
end;
