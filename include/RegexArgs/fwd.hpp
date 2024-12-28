#pragma once

#include <functional>
#include <string>
#include <vector>

namespace args {

using ArgsContainer_t = std::vector<std::string>;
class Argv;

namespace resp {
    struct RespContext;
    using Response_t = std::function<void(RespContext)>;
}

}
