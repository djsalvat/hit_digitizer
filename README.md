# Hit Digitizer

Modern nuclear and particle physics experiments are subject to high rates
and require precise measurements of physical observables.
Many experiments extract and store digital waveforms
from the various interactions within detectors,
which in turn requires sophisticated waveform analysis techiques.
Monte Carlo simulations often provide estimates of
the magnitude of systematic uncertainties,
and there is increasing need for translating Monte Carlo events
into synthetic waveforms to put analysis techniques to the test.

The `hit_digitizer` framework is a set of template classes
expecting a concrete type for
* a detector hit (containing, for example, energy, time, or particle type)
* a digitizer sample (such as an unsigned short, corresponding to 16-bit digitizer).
The HitDigitizer will create a "blank" waveform based upon the user-provided pedestal or baseline.
A JSON file provides the relevant parameters associated with the digitizer,
such as sampling rate and trace length.
The user then provides, for each channel of the detector indexed by integer:
* baselines
* A function which modifies the list of hits in each channel,
  potentially adding correlated hits in other channels
  to model cross-talk.
* A function which modifies the blank waveform to add noise
* A response function which takes the hit and provides the shape
  and magnitude to be added to the waveform.

This produces a map of channel number to synthetic waveform.
