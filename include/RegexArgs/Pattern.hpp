#include <string>

namespace args {
namespace pat {
    struct Pattern {
        std::string basic, times;

        template <typename T1>
        Pattern(T1&& basic_pattern)
            : basic(std::forward<T1>(basic_pattern))
        {
        }

        template <typename T1, typename T2>
        Pattern(T1&& basic_pattern, T2&& times_pattern)
            : basic(std::forward<T1>(basic_pattern))
            , times(std::forward<T2>(times_pattern))
        {
        }

        std::string pattern() const { return "(?:" + basic + ")" + times; }
        operator std::string() const { return pattern(); }
    };

    inline const Pattern equal(R"(\=(\S+))");

    inline const Pattern normal(R"(\s+([^-\s]+))", "+");

    inline const Pattern suffix(R"(\S+)");

    inline const Pattern flag("");
}
}
