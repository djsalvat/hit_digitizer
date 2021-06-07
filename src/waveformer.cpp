#include "waveformer.h"
#include "nlohmann/json.hpp"
#include <string>
#include <fstream>

using json = nlohmann::json;

namespace waveformer
{
    digitizer_parameters get_digitizer_parameters(std::string filename)
    {
        std::ifstream fin(filename.c_str());
        json j;
        fin >> j;
        digitizer_parameters dp = {
                                    j["frequency"],
                                    j["units"],
                                    j["pretrace_length"],
                                    j["trace_length"]
                                  };
        return dp;
    }

    template<typename hit,typename sample>
    Waveformer<hit,sample>::Waveformer(
                            digitizer_parameters p,
                            channel_baselines<sample> bl,
                            Response<hit,sample> r,
                            NoiseGenerator<sample> ng,
                            CrossTalker<hit> ct)
                          : params(p), baselines(bl),
                            response(r), noise(ng), crosstalk(ct)
    {
        
    }

    template<typename hit,typename sample>
    waveform<sample> Waveformer<hit,sample>::create_waveform(double time,sample baseline)
    {
        return waveform<sample>(
                                time,
                                std::list<hit>(params.trace_length,baseline)
                               );
    }

    template<typename hit,typename sample>
    event<sample> Waveformer<hit,sample>::operator()(channel_hits<hit> hits)
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
    }

    Test::Test()
    {
    }
}
