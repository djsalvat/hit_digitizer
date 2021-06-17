#ifndef WAVEFORMER_H
#define WAVEFORMER_H

#include <list>
#include <vector>
#include <map>
#include <functional>
#include <utility>
#include <random>
#include <chrono>

namespace hit_digitizer
{
    //A list of hits with all necessary information
    //(energy, time, PSD parameter, etc...)
    //which is needed to simulate an individual waveform
    template<typename hit>
    using hit_list = std::pair<double,std::list<hit>>;
    //For a given event, which "channel hits" (indexed by an integer)
    //contained which hits
    template<typename hit>
    using channel_hits = std::map< int, hit_list<hit>>;
    //A cross-talk algorithm takes channel hits,
    //and uses the indices in some way
    //to affect hits in other channels
    template<typename hit>
    using cross_talk = std::function<void(channel_hits<hit>&)>;
    //For asynchronous triggers, create a "hardware-level"
    //trigger condition
    template<typename hit>
    using hit_trigger = std::function<bool(const hit_list<hit>&)>;
    //A waveform is a global trigger time and vector of samples
    template<typename sample>
    using waveform = std::pair<double, std::vector<sample>>;
    //each channel can have a baseline offset
    template<typename sample>
    using channel_baselines = std::map<int,sample>;
    //A noise generator produces a blank waveform
    //with any baseline noise
    template<typename sample>
    using noise_generator = std::function<void(waveform<sample>&)>;
    //a response_function takes the relevant hits, and modifies the waveforms
    template<typename hit, typename sample>
    using response_function = std::function<void(waveform<sample>&,const hit_list<hit>&)>;
    //An event is a list of waveforms
    //referenced by channel
    template<typename sample>
    using event = std::map<int,waveform<sample>>;

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
    class HitDigitizer
    {
    public:
        HitDigitizer(
                   digitizer_parameters p,
                   channel_baselines<sample> bl,
                   response_function<hit,sample> r,
                   noise_generator<sample> ng,
                   cross_talk<hit> ct)
                   : params(p), baselines(bl),
                     response(r), noise(ng),
                     crosstalk(ct) {};
       ~HitDigitizer(){};

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
        response_function<hit,sample> response;
        noise_generator<sample> noise;
        cross_talk<hit> crosstalk;
        //helper method to generate a blank waveform.
        waveform<sample> create_waveform(double time,sample baseline)
        {
            return waveform<sample>(
                                    time,
                                    std::vector<sample>(params.trace_length,baseline)
                                   );
        };
    };

    namespace noise_generators
    {
        //Simple gauss noise with variance sigma.
        template<typename sample>
        class Gaussian
        {
            public:
                Gaussian(sample s, digitizer_parameters dp)
                             : sigma(s),dig_params(dp),
                               generator(std::chrono::system_clock::now().time_since_epoch().count()),
                               distribution(0.0,float(sigma))
                               {};
                void operator()(waveform<sample>& wf)
                {
                    for (auto& s : wf.second) s += distribution(generator);
                };
            private:
                sample sigma;
                digitizer_parameters dig_params;
                std::default_random_engine generator;
                std::normal_distribution<float> distribution;
        };
        
    }
}

#endif
