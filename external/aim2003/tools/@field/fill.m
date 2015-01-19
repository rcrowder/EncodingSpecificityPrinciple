function f=fill(f,x1,x2,wert)
% Fill the part of the matrix from x1 to x2 with that value
% The size of the matrix might be increased

nrfre=getnrfrequencies(f);
for x=x1:x2  
    nullen=ones(1,nrfre)*wert;
    sig=signal(nullen);
    f=setcolumn(f,x,sig);
end

