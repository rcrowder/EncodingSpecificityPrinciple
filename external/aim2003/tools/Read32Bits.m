%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% File:			Read32Bits.m
% Purpose:		Read four 8 bit bytes and combine according to the machine's
%				endian architecture.
% Comments:	
% Author:		L. P. O'Mard
% Revised by:
% Created:
% Updated:
% Copyright:	(c) 2000, University of Essex
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%

function data=Read32Bits(fid, littleEndian)

first = Read16Bits(fid, littleEndian);
second = Read16Bits(fid, littleEndian);
if (littleEndian == 0)
	data = bitshift(first, 16) + second;
else
	data = bitshift(second, 16) + first;
end;
