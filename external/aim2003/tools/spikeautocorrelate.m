

function cor=spikeautocorrelate(data,sr,window)
% calculate the autocorrelation (all interval histogram)
% for the data in data with the samplerate in sr, but only the time
% window given my window

nr_dat=length(data);

if nr_dat<2
	cor=[];
	return
end

cor=zeros(1,round(window*sr));

for ii=1:nr_dat-1
	dat1=data(ii);
	for jj=ii+1:nr_dat
		dat2=data(jj);
		len=(dat2-dat1)*sr;
		indx=round(len);
		if indx < window*sr && indx >= 1
			cor(indx)=cor(indx)+1;
		end
	end			
end
