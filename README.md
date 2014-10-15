Vestibulocochlear Nerve
=======================

The purpose of this repository is to investigate the encoding of auditory signals (cochlear origin) and it's passage into the primary auditory cortex. 

http://en.wikipedia.org/wiki/Vestibulocochlear_nerve

An intention is to use the [Numenta Platform for Intelligent Computing](http://numenta.org/ "Numenta | NuPIC") library for high level abstraction and inference (only PreFrontal and T2/T3?). Working down via the;  
* Medial geniculate nucleus (MGB to Brodmann area 41 (A1)), 
* Inferior colliculus (Midbrain), 
* Lateral lemniscus nuclei (Pons), 
* Superior olivary nuclei (Pons), 
* to the Cochlear nuclei (Medulla).

But firstly, the creation of the SDR encoders.

### Encoding for the Thalamocortical pathway

Before reaching the ventral division of the medial geniculate body of the thalamus (MGB), a variety of information processing needs to take place. The MGB feeds into the primary auditory cortex (A1, NuPIC HTM). 

http://en.wikipedia.org/wiki/Neuronal_encoding_of_sound

#### AIM - The Auditory Image Model
http://www.pdn.cam.ac.uk/groups/cnbh/research/aim.php  
http://www.acousticscale.org/wiki/index.php/Main_Page  

##### Processing stages in AIM

The principle functions of AIM are to describe and simulate:

1. Pre-cochlear processing (PCP) of the sound up to the oval window of the cochlea,
2. Basilar membrane motion (BMM) produced in the cochlea,
3. The neural activity pattern (NAP) observed in the auditory nerve and cochlear nucleus,
4. The identification of maxima in the NAP that strobe temporal integration (STI),
5. The construction of the stabilized auditory image (SAI) that forms the basis of auditory perception,
6. A size invariant representation of the information in the SAI referred to as the Mellin Magnitude Image (MMI).

Outer middle ear processing (PCP) can be approximated with 2nd order Butterworth filter? (Lf 450 Hz, Hf 8000 Hz)

Need to model/account for?  
* Oligodendrocytes variation between cortical regions?  
* Astrocyte calcium release and propogation events between cortical layers?  
* Astrocytic glial cell effects on the modulation of synaptic transmission and promotion of the myelinating activity of oligodendrocytes  
* Martinotti cell cortical dampening mechanism?  

http://en.wikipedia.org/wiki/Spectral_density  
http://en.wikipedia.org/wiki/Critical_band  
http://en.wikipedia.org/wiki/Equal-loudness_contour  
http://en.wikipedia.org/wiki/Hair_cell#Neural_connection  
http://en.wikipedia.org/wiki/Nyquist%E2%80%93Shannon_sampling_theorem  

Auditory abstraction from spectro-temporal features to coding auditory entities  
http://www.ncbi.nlm.nih.gov/pmc/articles/PMC3503225/

The audible frequency range for adult humans is about 20 Hz to 16,000 Hz. With about 32,000 hair cells per ear in the cochlear. 
Maximum frequency 20k Hz for infants, 16k Hz for adults. Within this range, the human ear is most sensitive between 2 and 5 kHz, largely due to the resonance of the ear canal and the transfer function of the ossicles of the middle ear.

Via mechanotransduction (see organ of Corti, below), inner hair cells detect the motion of waves, and excite myelinated Type I neurons of the auditory nerve. Exocytosis of neurotransmitter vesicles at ribbon synapses, generates an action potential in the connected auditory-nerve fiber. 

The mechanotransduction by stereocilia is highly sensitive and able to detect perturbations as small as fluid fluctuations of 0.3 nanometers, and can convert this mechanical stimulation into an electrical nerve impulse in about 10 microseconds. 

Outer hair cells on the other hand boost the mechanical signal by using electromechanical feedback, a frequency specific boost and unmyelinated. Fast enough feedback from unmyelinated cortical neurons to handle attentional auditory processing? (In the inferior colliculus?)

Feedback from the tensor tympani and stapedius muscles could also be determined, to deduce energy state information from ossicle simulation.

The basilar membrane of the cochlea is tonotopic.

* Fourier Transform.  
* Power Spectrum.  
* Mipmap sub-sampling to add to receptive field.  
* Tuning on the basilar membrane changes from high to low frequency.  
  * Auditory filters that are non-linear, level-dependent.
  * Filter critical bandwidth size decreases from the base to apex of the cochlea (high to low)
  * Need a Glasberg and Moore approximation equivalent rectangular bandwidth (ERB)?  
  * A simple linear gammatone filter?  

Thalamic reticular nucleus (TRN) neurons, targets medial geniculate body (MGB), may transiently deactivate surrounding TRN neurons in response to fresh stimulus, altering auditory thalamus responses and inducing attention shift.  

- Spectro-Temporal Receptive Field (STRF) estimated from the responses to relevant complex stimuli  
- Volterra integral representation?  
- Processing of spatial cues all pre-isothalamic?  
- Structure cortical region BA44 & Meaning BA47  
- Planum Temporale involved in absolute pitch (AP)?  
- Pars Orbitalis, temporal coherence in Brodmann area 47?  
- Speech and music divergence (phoneme recognition)?  

* Feature extraction (150 - 400 ms)  
  * relative piches (~1500 frequencies),  
  * peak absolute pitch, 
  * loudness, 
  * timbre, 
  * spatial location, 
  * reverb, 
  * tone duration, 
  * note onset times  
* Feature inegration (100 - 150 ms)  

### The pathways of our hearing

![The pathways of our hearing](doc/photos_2701AC3F-EAE2-4579-8E2E-B360D81D22A8.gif "http://www.edoctoronline.com/medical-atlas.asp?c=4&id=21934&m=2")

### Anatomy of the Human Ear

http://en.wikibooks.org/wiki/Sensory_Systems/Auditory_System

<p><a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/d/d2/Anatomy_of_the_Human_Ear.svg/1200px-Anatomy_of_the_Human_Ear.svg.png" alt="Anatomy of the Human Ear.svg" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg">Anatomy of the Human Ear</a>" by Chittka L, Brockmann A - <a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=get-document&amp;doi=10.1371/journal.pbio.0030137&amp;ct=1">Perception Space—The Final Frontier, A PLoS Biology Vol. 3, No. 4, e137 doi:10.1371/journal.pbio.0030137</a> (<a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=slideshow&amp;type=figure&amp;doi=10.1371/journal.pbio.0030137&amp;id=24748">Fig. 1A</a>/<a rel="nofollow" class="external text" href="http://www.plosbiology.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pbio.0030137.g001">Large version</a>), vectorised by <a href="//commons.wikimedia.org/wiki/User:Inductiveload" title="User:Inductiveload">Inductiveload</a>. Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Organ of corti

<p><a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/Organ_of_corti.svg/1200px-Organ_of_corti.svg.png" alt="Organ of corti.svg" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg">Organ of corti</a>" by <a href="//commons.wikimedia.org/wiki/User:Madhero88" title="User:Madhero88">Madhero88</a><span class="int-own-work"></span> Licensed under <a href="http://creativecommons.org/licenses/by-sa/3.0" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Uncoiled cochlea with basilar membrane

<p><a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/6/65/Uncoiled_cochlea_with_basilar_membrane.png/1200px-Uncoiled_cochlea_with_basilar_membrane.png" alt="Uncoiled cochlea with basilar membrane.png" width="500"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png">Uncoiled cochlea with basilar membrane</a>" by Kern A, Heid C, Steeb W-H, Stoop N, Stoop R - <a rel="nofollow" class="external text" href="http://www.ploscompbiol.org/article/info:doi/10.1371/journal.pcbi.1000161">Biophysical Parameters Modification Could Overcome Essential Hearing Gaps</a> (<a rel="nofollow" class="external autonumber" href="http://www.ploscompbiol.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pcbi.1000161.g002">[1]</a>). Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>
