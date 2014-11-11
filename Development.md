Development
===========

Need to find this for Raspberry PI  
Beaglebone: Qt Creator for C++ ARM Embedded Linux Development http://youtu.be/kP7uvOu9hoQ  

### Third-party Applications (TBD)

* [Sonic Visualiser](http://www.sonicvisualiser.org/)
* [Vamp Plugins](http://www.vamp-plugins.org/)
* [Auditory Image Model](http://www.acousticscale.org/wiki/index.php/Main_Page)
* [Numenta CLA & HTM](http://numenta.org/)

Some example Vamp plugins for SV can be found here; http://www.mazurka.org.uk/software/sv/plugin/

### AIM - The Auditory Image Model

http://www.pdn.cam.ac.uk/groups/cnbh/research/aim.php  
https://code.google.com/p/aimc/  

For an overview of AIM, see [here](http://www.acousticscale.org/wiki/index.php/Overview)

#### Elements of AIM

A high level breakdown of AIM, described in the [Wiki](http://www.acousticscale.org/wiki/index.php/Category:Auditory_Image):

1. [Communication Sounds and Acoustic Scale](http://www.acousticscale.org/wiki/index.php/Part_1_Chapters)
2. [The Auditory Image and Auditory Figures](http://www.acousticscale.org/wiki/index.php/Part_2_Chapters)
3. [Auditory Events](http://www.acousticscale.org/wiki/index.php/Part_3_Chapters)
4. [The Size-Covariant Auditory Image](http://www.acousticscale.org/wiki/index.php/Part_4_Chapters)
5. [Auditory Objects and the Auditory Scene](http://www.acousticscale.org/wiki/index.php/An_introduction_to_auditory_objects,_events,_figures,_images_and_scenes)
6. Auditory Image Research

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
* A1 neurons carrying six times less information about ST then IC
* Abstract entities per spike is twice smaller in A1 than IC
