Cochlear Nerve
==============

The purpose of this repository is to investigate the sparse encoding of auditory signals ([Cochlear nucleus](http://en.wikipedia.org/wiki/Cochlear_nucleus)), and it's passage into the primary auditory cortex. 

http://en.wikipedia.org/wiki/Vestibulocochlear_nerve

An intention is to use the [Numenta Platform for Intelligent Computing](http://numenta.org/ "Numenta | NuPIC") library at the highest level (only PreFrontal, and T2/T3?). Working down via the;  
* [Medial geniculate nucleus](http://en.wikipedia.org/wiki/Medial_geniculate_nucleus) (MGB to Brodmann area 41 (A1)), [Ventral posterior nucleus](http://en.wikipedia.org/wiki/Ventral_posterior_nucleus)
* [Inferior colliculus](http://en.wikipedia.org/wiki/Inferior_colliculus) (Midbrain), 
* [Lateral lemniscus nuclei](http://en.wikipedia.org/wiki/Lateral_lemniscus) (Pons), 
* [Superior olivary complex](http://en.wikipedia.org/wiki/Superior_olivary_complex) (Pons), 
* to the [Cochlear nucleus](http://en.wikipedia.org/wiki/Cochlear_nucleus) (Medulla).

###### Suggested reading list

[Spectral density](http://en.wikipedia.org/wiki/Spectral_density)  
[Critical band](http://en.wikipedia.org/wiki/Critical_band)  
[Equal loudness contour](http://en.wikipedia.org/wiki/Equal-loudness_contour)   
[Hair cell - Neural connection](http://en.wikipedia.org/wiki/Hair_cell#Neural_connection)  
[Nyquist–Shannon sampling theorem](http://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem)  

- 
Auditory-Based Processing of Communication Sounds  
Walters, Thomas C. (2011)  
http://scholar.google.co.uk/scholar?q=Auditory-Based+Processing+of+Communication+Sounds
- 
Patterning of pre-thalamic somatosensory pathways  
Gabrielle Pouchelon, Laura Frangeul, Filippo M. Rijli and Denis Jabaudon (2012)  
http://scholar.google.co.uk/scholar?q=Patterning+of+pre-thalamic+somatosensory+pathways&hl=en&as_sdt=0&as_vis=1&oi=scholart&sa=X&ei=padSVPTzGsLhaPOjgagH&ved=0CCAQgQMwAA  

See section _Ascending trigeminothalamic pathways_, follows _From whiskers to the brainstem trigeminal complex_.  
And Fig 1. Summary of pre-thalamic somatosensory pathways.

- 
Time‐domain modeling of peripheral auditory processing: A modular architecture and a software platform  
Roy D. Patterson1, Mike H. Allerhand1 and Christian Giguère (1995)  
http://scitation.aip.org/content/asa/journal/jasa/98/4/10.1121/1.414456  
http://scholar.google.co.uk/scholar?q=time+domain+modelling+of+peripheral+auditory+processing
- 
Binary Spiking in Auditory Cortex  
Michael R. DeWeese, Michael Wehr, and Anthony M. Zador (2003)  
http://scholar.google.co.uk/scholar?q=Binary+Spiking+in+Auditory+Cortex
- 
Efficient coding of natural sounds  
Michael S. Lewicki (2012)  
http://scholar.google.co.uk/scholar?q=Efficient+coding+of+natural+sounds
- 
Auditory abstraction from spectro-temporal features to coding auditory entities  
Gal Chechik and Israel Nelken (2012)  
http://www.pnas.org/content/109/46/18968.short  
http://www.ncbi.nlm.nih.gov/pmc/articles/PMC3503225/  
- 
Steady-state evoked potentials as an index of multisensory temporal binding  
Sylvie Nozaradan, b, Isabelle Peretz, André Mouraux (2011)  
http://scholar.google.co.uk/scholar?q=Steady-state+evoked+potentials+as+an+index+of+multisensory+temporal+binding


But firstly, the creation of the SDR encoders.

### Encoding for the Thalamocortical pathways

Before reaching the ventral division of the medial geniculate body of the thalamus (MGB), a variety of information processing needs to take place. A [sparse distributed representation](https://www.youtube.com/watch?v=LbZtc_zWBS4) ([SDR](https://github.com/numenta/nupic/wiki/Sparse-Distributed-Representations)), an array containing distributed bit encoded semantic meanings, is required to be sent into the Thalamocortical highways. Our highways will be built using NuPIC. 

http://en.wikipedia.org/wiki/Neuronal_encoding_of_sound

#### AIM - The Auditory Image Model

http://www.pdn.cam.ac.uk/groups/cnbh/research/aim.php  
http://www.acousticscale.org/wiki/index.php/Main_Page  
https://code.google.com/p/aimc/  

##### Processing stages in AIM

The principle functions of AIM are to simulate:

1. Pre-cochlear processing (PCP) of the sound up to the oval window of the cochlea,
2. Basilar membrane motion (BMM) produced in the cochlea,
3. The neural activity pattern (NAP) observed in the auditory nerve and cochlear nucleus,
4. The identification of maxima in the NAP that strobe temporal integration (STI),
5. The construction of the stabilized auditory image (SAI) that forms the basis of auditory perception,

Optionally; A size invariant representation of the information in the SAI referred to as the Mellin Magnitude Image (MMI).

The audible frequency range for adult humans is about 20 Hz to 16,000 Hz. With about 32,000 hair cells per ear in the cochlear. Maximum frequency 20k Hz for infants, 16k Hz for adults. Within this range, the human ear is most sensitive between 2 and 5 kHz, largely due to the resonance of the ear canal and the transfer function of the ossicles of the middle ear.

Via mechanotransduction (see organ of Corti, below), inner hair cells detect the motion of waves, and excite myelinated Type I neurons of the auditory nerve. Exocytosis of neurotransmitter vesicles at ribbon synapses, generates an action potential in the connected auditory-nerve fiber. 

The mechanotransduction by stereocilia is highly sensitive, and able to detect perturbations as small as fluid fluctuations of 0.3 nanometers. Converting this mechanical stimulation into an electrical nerve impulse in about 10 microseconds. 

Outer hair cells on the other hand boost the mechanical signal by using electromechanical feedback, a frequency specific boost and unmyelinated.  

Outer middle ear processing (PCP) can be approximated with 2nd order Butterworth filter? (Lf 450 Hz, Hf 8000 Hz)

The basilar membrane of the cochlea is tonotopic.  

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
  * Auditory filters that are non-linear, level-dependent.
  * Filter critical bandwidth size decreases from the base to apex of the cochlea (high to low)

Can feedback from unmyelinated cortical neurons handle attentional auditory processing? Via the inferior colliculus?

Feedback from the tensor tympani and stapedius muscles could also be determined to deduce energy state information from ossicle simulation?

Thalamic reticular nucleus (TRN) neurons (targets medial geniculate body (MGB)), may transiently deactivate surrounding TRN neurons in response to fresh stimulus, altering auditory thalamus responses and inducing attention shift?  

- Spectro-Temporal Receptive Field (STRF) estimated from the responses to relevant complex stimuli  
- Volterra integral representation?  
- Processing of spatial cues all pre-isothalamic?  
- Planum Temporale involved in absolute pitch (AP)?  
- Structure cortical region BA44 & Meaning BA47  
- Pars Orbitalis, temporal coherence in Brodmann area 47?  
- Speech and music divergence (phoneme recognition)?  

### The pathways of our hearing

![The pathways of our hearing](doc/photos_2701AC3F-EAE2-4579-8E2E-B360D81D22A8.gif "http://www.edoctoronline.com/medical-atlas.asp?c=4&id=21934&m=2")

### Anatomy of the Human Ear

http://en.wikibooks.org/wiki/Sensory_Systems/Auditory_System

<p><a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/d/d2/Anatomy_of_the_Human_Ear.svg/1200px-Anatomy_of_the_Human_Ear.svg.png" alt="Anatomy of the Human Ear.svg" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg">Anatomy of the Human Ear</a>" by Chittka L, Brockmann A - <a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=get-document&amp;doi=10.1371/journal.pbio.0030137&amp;ct=1">Perception Space—The Final Frontier, A PLoS Biology Vol. 3, No. 4, e137 doi:10.1371/journal.pbio.0030137</a> (<a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=slideshow&amp;type=figure&amp;doi=10.1371/journal.pbio.0030137&amp;id=24748">Fig. 1A</a>/<a rel="nofollow" class="external text" href="http://www.plosbiology.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pbio.0030137.g001">Large version</a>), vectorised by <a href="//commons.wikimedia.org/wiki/User:Inductiveload" title="User:Inductiveload">Inductiveload</a>. Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Organ of corti

<p><a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/Organ_of_corti.svg/1200px-Organ_of_corti.svg.png" alt="Organ of corti.svg" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg">Organ of corti</a>" by <a href="//commons.wikimedia.org/wiki/User:Madhero88" title="User:Madhero88">Madhero88</a><span class="int-own-work"></span> Licensed under <a href="http://creativecommons.org/licenses/by-sa/3.0" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Uncoiled cochlea with basilar membrane

<p><a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/6/65/Uncoiled_cochlea_with_basilar_membrane.png/1200px-Uncoiled_cochlea_with_basilar_membrane.png" alt="Uncoiled cochlea with basilar membrane.png" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png">Uncoiled cochlea with basilar membrane</a>" by Kern A, Heid C, Steeb W-H, Stoop N, Stoop R - <a rel="nofollow" class="external text" href="http://www.ploscompbiol.org/article/info:doi/10.1371/journal.pcbi.1000161">Biophysical Parameters Modification Could Overcome Essential Hearing Gaps</a> (<a rel="nofollow" class="external autonumber" href="http://www.ploscompbiol.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pcbi.1000161.g002">[1]</a>). Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>
