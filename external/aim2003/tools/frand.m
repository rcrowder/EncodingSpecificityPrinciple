function r=frand(from,to)
% returns a random number in that range

range=to-from;
r=from+rand*range;