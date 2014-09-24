Vestibulocochlear Nerve
=======================

The purpose of this repository is to investigate the encoding of auditory signals (cochlear origin) and it's passage into the primary auditory cortex. 

http://en.wikipedia.org/wiki/Vestibulocochlear_nerve

![The pathways of our hearing](doc/photos_2701AC3F-EAE2-4579-8E2E-B360D81D22A8.gif "http://www.edoctoronline.com/medical-atlas.asp?c=4&id=21934&m=2")

An intention is to use the [Numenta Platform for Intelligent Computing](http://numenta.org/ "Numenta | NuPIC") library for high level abstraction and inference (only PreFrontalCortex and T2/T3?). Working down via the Medial geniculate nucleus (MGB, Brodmann area 41 (A1)), Inferior colliculus (Midbrain), the lateral lemniscus nuclei (Pons), Superior olivary nuclei (Pons), to the Cochlear nuclei (Medulla).

But firstly, the creation of the SDR encoders.

### Anatomy of the Human Ear

http://en.wikibooks.org/wiki/Sensory_Systems/Auditory_System

<p><a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/d/d2/Anatomy_of_the_Human_Ear.svg/1200px-Anatomy_of_the_Human_Ear.svg.png" alt="Anatomy of the Human Ear.svg"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Anatomy_of_the_Human_Ear.svg#mediaviewer/File:Anatomy_of_the_Human_Ear.svg">Anatomy of the Human Ear</a>" by Chittka L, Brockmann A - <a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=get-document&amp;doi=10.1371/journal.pbio.0030137&amp;ct=1">Perception Space—The Final Frontier, A PLoS Biology Vol. 3, No. 4, e137 doi:10.1371/journal.pbio.0030137</a> (<a rel="nofollow" class="external text" href="http://biology.plosjournals.org/perlserv/?request=slideshow&amp;type=figure&amp;doi=10.1371/journal.pbio.0030137&amp;id=24748">Fig. 1A</a>/<a rel="nofollow" class="external text" href="http://www.plosbiology.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pbio.0030137.g001">Large version</a>), vectorised by <a href="//commons.wikimedia.org/wiki/User:Inductiveload" title="User:Inductiveload">Inductiveload</a>. Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Organ of corti

<p><a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/Organ_of_corti.svg/1200px-Organ_of_corti.svg.png" alt="Organ of corti.svg"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Organ_of_corti.svg#mediaviewer/File:Organ_of_corti.svg">Organ of corti</a>" by <a href="//commons.wikimedia.org/wiki/User:Madhero88" title="User:Madhero88">Madhero88</a> - <span class="int-own-work">Own work</span>. Licensed under <a href="http://creativecommons.org/licenses/by-sa/3.0" title="Creative Commons Attribution-Share Alike 3.0">CC BY-SA 3.0</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>

### Uncoiled cochlea with basilar membrane

<p><a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png"><img src="http://upload.wikimedia.org/wikipedia/commons/thumb/6/65/Uncoiled_cochlea_with_basilar_membrane.png/1200px-Uncoiled_cochlea_with_basilar_membrane.png" alt="Uncoiled cochlea with basilar membrane.png"></a><br>"<a href="http://commons.wikimedia.org/wiki/File:Uncoiled_cochlea_with_basilar_membrane.png#mediaviewer/File:Uncoiled_cochlea_with_basilar_membrane.png">Uncoiled cochlea with basilar membrane</a>" by Kern A, Heid C, Steeb W-H, Stoop N, Stoop R - <a rel="nofollow" class="external text" href="http://www.ploscompbiol.org/article/info:doi/10.1371/journal.pcbi.1000161">Biophysical Parameters Modification Could Overcome Essential Hearing Gaps</a> (<a rel="nofollow" class="external autonumber" href="http://www.ploscompbiol.org/article/showImageLarge.action?uri=info%3Adoi%2F10.1371%2Fjournal.pcbi.1000161.g002">[1]</a>). Licensed under <a href="http://creativecommons.org/licenses/by/2.5" title="Creative Commons Attribution 2.5">CC BY 2.5</a> via <a href="//commons.wikimedia.org/wiki/">Wikimedia Commons</a>.</p>
