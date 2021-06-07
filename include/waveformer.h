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
    using hitlist = std::pair<double,std::list<hit> >;
    //For a given event, which "channel hits" (indexed by an integer)
    //contained which hits
    template<typename hit>
    using channel_hits = std::map< int, hitlist<hit> >;
    //A cross-talk algorithm takes channel hits,
    //and uses the indices in some way
    //to affect hits in other channels
    template<typename hit>
    using CrossTalker = std::function<void(channel_hits<hit>&)>;
    //For asynchronous triggers, create a "hardware-level"
    //trigger condition
    template<typename hit>
    using HitTrigger = std::function<bool(const hitlist<hit>&)>;
    //A waveform is a global trigger time and list of samples
    template<typename sample>
    using waveform = std::pair<double, std::list<sample> >;
    //each channel can have a baseline offset
    template<typename sample>
    using channel_baselines = std::map<int,sample>;
    //A noise generator produces a blank waveform
    //with any baseline noise
    template<typename sample>
    using NoiseGenerator = std::function<void(waveform<sample>&)>;
    //A response applies the noise generator to produce the blank waveform,
    //along with the relevant hits, and populates the waveforms
    template<typename hit, typename sample>
    using Response = std::function<void(waveform<sample>&,const hitlist<hit>&)>;
    //An event is a list of waveforms
    //referenced by channel
    template<typename sample>
    using event = std::map<int,waveform<sample> >;

    typedef struct
    {
        double frequency;
        std::string units;
        int pretrace_length;
        int trace_length;
    } digitizer_parameters;

    digitizer_parameters get_digitizer_parameters(std::string filename);

    //Take the above ingredients,
    //wrap into a class.
    //The () operator takes the channel hits,
    //produces the set of desired waveforms.
    template<typename hit,typename sample>
    class Waveformer
    {
    public:
        Waveformer(
                   digitizer_parameters p,
                   channel_baselines<sample> bl,
                   Response<hit,sample> r,
                   NoiseGenerator<sample> ng,
                   CrossTalker<hit> ct)
                   : params(p), baselines(bl),
                     response(r), noise(ng),
                     crosstalk(ct) {};
       ~Waveformer(){};

        event<sample> operator()(channel_hits<hit> hits)
        {
            crosstalk(hits);

            event<sample> e;
            for (auto const& h : hits)
            {
                auto c = h.first;
                auto hl = h.second; 

                e[c] = create_waveform(hl.first,baselines[c]);
                noise(e[c]);
                response(e[c],hl);
            }

            return e;
        };
    private:
        digitizer_parameters params;
        channel_baselines<sample> baselines;
        Response<hit,sample> response;
        NoiseGenerator<sample> noise;
        CrossTalker<hit> crosstalk;
        //helper method to generate a blank waveform.
        waveform<sample> create_waveform(double time,sample baseline)
        {
            return waveform<sample>(
                                    time,
                                    std::list<sample>(params.trace_length,baseline)
                                   );
        };
    };
}

#endif
