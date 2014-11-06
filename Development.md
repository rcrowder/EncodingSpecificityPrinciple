Development
===========

Need to find this for Raspberry PI  
Beaglebone: Qt Creator for C++ ARM Embedded Linux Development http://youtu.be/kP7uvOu9hoQ  

### AIM - The Auditory Image Model

http://www.pdn.cam.ac.uk/groups/cnbh/research/aim.php  
http://www.acousticscale.org/wiki/index.php/Main_Page  
https://code.google.com/p/aimc/  

#### Processing stages in AIM

The principle functions of AIM are to simulate:

1. Pre-cochlear processing (PCP) of the sound up to the oval window of the cochlea,
2. Basilar membrane motion (BMM) produced in the cochlea,
3. The neural activity pattern (NAP) observed in the auditory nerve and cochlear nucleus,
4. The identification of maxima in the NAP that strobe temporal integration (STI),
5. The construction of the stabilized auditory image (SAI) that forms the basis of auditory perception,

The audible frequency range for adult humans is about 20 Hz to 16,000 Hz. With about 32,000 hair cells per ear in the cochlear. Maximum frequency 20k Hz for infants, 16k Hz for adults. Within this range, the human ear is most sensitive between 2 and 5 kHz, largely due to the resonance of the ear canal and the transfer function of the ossicles of the middle ear.

Outer middle ear processing (PCP) can be approximated with 2nd order Butterworth filter? (Lf 450 Hz, Hf 8000 Hz)

Feature extraction (150 - 400 ms)  
 * relative piches (~1500 frequencies),  
 * peak absolute pitch, 
 * loudness, 
 * timbre, 
 * spatial location, 
 * reverb, 
 * tone duration, 
 * note onset times  

followed by,  
Feature inegration (100 - 150 ms)  

* Tuning on the basilar membrane changes from high to low frequency.  

