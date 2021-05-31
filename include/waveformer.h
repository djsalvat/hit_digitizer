#ifndef WAVEFORMER_H
#define WAVEFORMER_H

#include <list>
#include <map>
#include <functional>
#include <utility>

namespace waveformer
{
    //A list of hits with all necessary information
    //(energy, time, PSD parameter, etc...)
    //which is needed to simulate an individual waveform
    template<typename hit>
    using hitlist = std::list<hit>;
    //For a given event, which "channels" (indexed by an integer)
    //contained which hits
    template<typename hit>
    using channels = std::map< int, hitlist<hit> >;
    //A cross-talk algorithm takes channel hits,
    //and uses the indices in some way
    //to affect hits in other channels
    template<typename hit>
    using CrossTalker = std::function<void(channels<hit>&)>;
    //For asynchronous triggers, create a "hardware-level"
    //trigger condition
    template<typename hit>
    using HitTrigger = std::function<bool(const hitlist<hit>&)>;
    //A waveform is a global trigger time and list of samples
    template<typename sample>
    using waveform = std::pair<double, std::list<sample> >;
    //each channel can have a baseline offset
    template<typename sample>
    using channel_baselines = std::map< int, sample >;
    //A noise generator produces a blank waveform
    //with any baseline noise
    template<typename sample>
    using NoiseGenerator = std::function<waveform<sample>(double)>;
    //A response applies the noise generator to produce the blank waveform,
    //along with the relevant hits, and populates the waveforms
    template<typename hit, typename sample>
    using Response = std::function<waveform<sample>(NoiseGenerator<sample>*,hitlist<hit>&)>;

    typedef struct
    {
        double frequency;
        std::string units;
        int pretrace_length;
        int trace_length;
    } digitizer_parameters;

    digitizer_parameters get_digitizer_parameters(std::string filename);
}

#endif
