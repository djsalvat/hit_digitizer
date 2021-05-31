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
}
