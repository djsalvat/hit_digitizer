#include "hit_digitizer.h"
#include <iostream>
#include <functional>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

using namespace hit_digitizer;
using namespace std;

typedef struct
{
    double energy;
    double time;
} simple_hit;

using HitList = hit_list<simple_hit>;
using ChannelHits = channel_hits<simple_hit>;
using CrossTalk = cross_talk<simple_hit>;
using Trigger = hit_trigger<simple_hit>;

using sample = unsigned short;
using Baselines = channel_baselines<sample>;
using Waveform = waveform<sample>;
using NoiseGenerator = noise_generator<sample>;
using Response = response_function<simple_hit,sample>;
using Event = event<sample>;

class Gaussian
{
    public:
        Gaussian(sample s, sample b, digitizer_parameters dp)
                     : sigma(s),dig_params(dp),
                       generator(std::chrono::system_clock::now().time_since_epoch().count()),
                       distribution(0.0,float(sigma))
                       {};
        void operator()(Waveform& wf)
        {
            for (auto& s : wf.second) s += distribution(generator);
        };
    private:
        sample sigma;
        digitizer_parameters dig_params;
        std::default_random_engine generator;
        std::normal_distribution<float> distribution;
};

void test_response(Waveform& wf,const HitList& hl)
{
}

int main()
{
    Waveform wf(0.0,std::vector<sample>(32,256));
    CrossTalk ct = [](ChannelHits& c){};
    Trigger tr = [](const HitList& h) -> bool {return true;};

    digitizer_parameters dp = get_digitizer_parameters("parameter_example.json");
    cout << "frequency: " << dp.frequency       << " " << dp.units << endl
         << "pretrace:  " << dp.pretrace_length                    << endl
         << "trace:     " << dp.trace_length                       << endl;

    Gaussian g(2,256,dp);
    NoiseGenerator ng = g;
    Response r = test_response;
    g(wf);
    for (auto& s : wf.second)
    {
        cout << s << " ";
    }
    cout << endl;

    Baselines bl = {{0,256},{1,160}};

    simple_hit h1 = {0.0,0.0};
    simple_hit h2 = {0.0,0.0};
    simple_hit h3 = {0.0,0.0};
    HitList hl1 = {10.3,{h1,h2,h3}};
    simple_hit h4 = {0.0,0.0};
    simple_hit h5 = {0.0,0.0};
    simple_hit h6 = {0.0,0.0};
    HitList hl2 = {17.9,{h4,h5,h6}};
    ChannelHits ch = {{0,hl1},{1,hl2}};

    HitDigitizer<simple_hit,sample> WF(
                                     dp,
                                     bl,
                                     r,
                                     g,
                                     ct
                                    );

    Event evt = WF(ch);
    for (auto& e : evt)
    {
        auto& c = e.first;
        auto& wf = e.second;
        auto& time = wf.first;
        auto& samples = wf.second;
        cout << c << " " << time;
        for (auto& s : samples)
        {
            cout << " " << s;
        }
        cout << endl;
    }

    return 0;
}
