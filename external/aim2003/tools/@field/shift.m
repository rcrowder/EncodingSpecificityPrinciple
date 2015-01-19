function f=shift(f,x1,x2)
% shift the part of the matrix with the beginning x1 to x2 
% The size of the matrix might be increased
% The rest is filled with zeros

% first, shift the rest to the right
d=getdata(f);
s=size(d);
stop=s(2);
stop2=x2-x1+stop;
d(:,x2:stop2)=d(:,x1:stop);

% fill the left part with zeros
d(:,1:x2-1)=0;

f=field(d);

