#pragma once

#include "Argv.hpp"
#include <iostream>

namespace args {
namespace resp {
    struct RespContext {
        Argv& opts;
        const std::string& opt;
        ArgsContainer_t& args;
    };

    inline void print(const RespContext& ctx)
    {
        std::cout << ctx.opt << ": ";
        for (const auto& e : ctx.args)
            std::cout << e << " ";
        std::cout << std::endl;
    }
}
}
