function ret=getpart(f,t_start,t_stop)

ret=f;


vals=f.werte;
srf=getsr(f);
binstart=time2bin(srf,t_start);
binstop=time2bin(srf,t_stop);

retvals=vals(:,binstart:binstop);

ret=setvalues(f,retvals);

