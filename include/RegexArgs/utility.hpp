#pragma once

#include <sstream>
#include <utility>

namespace args
{
namespace internal
{
    class StringStream
    {
        inline static thread_local std::stringstream ss;

    public:
        static std::stringstream& instance(auto&& str)
        {
            ss.str(std::forward<decltype(str)>(str));
            return ss;
        }

        static std::stringstream& instance()
        {
            ss.str("");
            return ss;
        }
    };

    inline std::string splice(std::size_t argc, const auto& argv, bool ignore_first)
    {
        auto& ss = StringStream::instance();

        for (std::size_t i = ignore_first; i < argc; ++i)
            ss << argv[i] << ' ';

        return ss.str();
    }
}
}
