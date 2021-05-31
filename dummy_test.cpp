#include "waveformer.h"
#include <iostream>
#include <functional>
#include <iostream>
#include <chrono>
#include <random>
#include <algorithm>

using namespace waveformer;
using namespace std;

typedef struct
{
    double energy;
    double time;
} simple_hit;

using HitList = hitlist<simple_hit>;
using Channels = channels<simple_hit>;
using CrossTalker_t = CrossTalker<simple_hit>;
using Trigger_t = HitTrigger<simple_hit>;

using sample = unsigned short;
using Waveform = waveform<sample>;
using NoiseGenerator_t = NoiseGenerator<sample>;
using Response_t = Response<simple_hit,sample>;

class Gaussian
{
    public:
        Gaussian(sample s, sample b, digitizer_parameters dp)
                     : sigma(s),baseline(b),dig_params(dp),
                       generator(std::chrono::system_clock::now().time_since_epoch().count()),
                       distribution(float(baseline),float(sigma))
                       {};
        Waveform operator()(double t)
        {
            Waveform w(t,dig_params.trace_length);
            for (auto& s : w.second) s = distribution(generator);
            return w;
        };
    private:
        sample sigma;
        sample baseline;
        digitizer_parameters dig_params;
        std::default_random_engine generator;
        std::normal_distribution<float> distribution;
};

Waveform test_response(NoiseGenerator_t* ng,
                       HitList& hl)
{
    auto wf = (*ng)(2600.0);
    return wf;
}

int main()
{
    HitList hl;
    Channels ch;
    Waveform wf;
    CrossTalker_t ct = [](Channels& c){return;};
    Trigger_t tr = [](const HitList& h) -> bool {return true;};

    digitizer_parameters dp = get_digitizer_parameters("parameter_example.json");
    cout << "frequency: " << dp.frequency       << " " << dp.units << endl
         << "pretrace:  " << dp.pretrace_length                    << endl
         << "trace:     " << dp.trace_length                       << endl;

    Gaussian g(2,256,dp);
    NoiseGenerator_t ng = g;
    Response_t r = test_response;
    Waveform w = g(1000.0);
    for (auto& s : w.second)
    {
        cout << s << " ";
    }
    cout << endl;

    return 0;
}
