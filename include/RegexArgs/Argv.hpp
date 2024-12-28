#pragma once

#include "Pattern.hpp"
#include "detail/Values.hpp"
#include "fwd.hpp"
#include "utility.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <format>
#include <string>
#include <unordered_map>
#include <utility>

#ifdef USE_BOOST_REGEX
#include <boost/regex.hpp>
#else
#include <regex>
#endif

namespace args
{

enum class Behavior : uint8_t
{
    OPTIONAL,
    DEFAULT,
    REQUIRED
};

class Argv final
{
    using OptsContainer_t = std::unordered_map<std::string, ArgsContainer_t>;

public:
    explicit Argv(auto&& cmdl, bool ignore_first = true)
        : _unresponded(std::forward<decltype(cmdl)>(cmdl))
    {
        if (ignore_first) remove_first();
    }

    Argv(int argc, const char* const argv[], bool ignore_first = true)
        : _unresponded(internal::splice(argc, argv, ignore_first))
    { }

    ~Argv() = default;

    template <Behavior B = Behavior::OPTIONAL>
    Argv& respond(auto&& opt, const pat::Pattern& pattern, const resp::Response_t& response = nullptr)
    {
#ifdef USE_BOOST_REGEX
        using boost::smatch, boost::regex, boost::regex_search, boost::sregex_iterator, boost::cregex_iterator, boost::regex_constants::match_not_null;
#else
        using std::smatch, std::regex, std::regex_search, std::sregex_iterator, std::cregex_iterator, std::regex_constants::match_not_null;
#endif
        smatch matches;
        const regex opt_r(std::format("(?:^|\\s+)({})({})\\s*", opt, pattern.pattern()));

        if (!regex_search(_unresponded, matches, opt_r, match_not_null)) {
            if constexpr (Behavior::REQUIRED == B) throw std::invalid_argument(std::format("{} is required", opt));
            else if constexpr (Behavior::DEFAULT == B) {
                auto& [_opt, args] = *opts.try_emplace(std::forward<decltype(opt)>(opt)).first;
                response({ *this, _opt, args });
            }
            return *this;
        }

        auto& [_opt, args] = *opts.try_emplace(std::forward<decltype(opt)>(opt)).first;

        if (matches[2].length()) { // do not push the null string.
            const regex arg_r(pattern.basic);
            sregex_iterator arg_iter(matches[2].first, matches[2].second, arg_r), end;
            for (; arg_iter != end; ++arg_iter)
                // starting from begin() + 1 are those groups that hold what we want
                for (auto&& arg = arg_iter->begin() + bool(arg_r.mark_count()); arg != arg_iter->end(); ++arg)
                    args.emplace_back(arg->str());
        }
        // remove this and spaces until the next to maintain readablity.
        _unresponded.erase(matches[1].first, matches[0].second);

        if (response) response({ *this, _opt, args });

        return *this;
    }

    Argv& reset(auto&& argv, bool ignore_first = true)
    {
        _unresponded.assign(std::forward<decltype(argv)>(argv));
        opts.clear();

        if (ignore_first) remove_first();

        return *this;
    }

    Argv& reset(int argc, const char* const argv[], bool ignore_first = true)
    {
        _unresponded.assign(internal::splice(argc, argv, ignore_first));
        opts.clear();

        return *this;
    }

    bool exists(const std::string& opt) const { return opts.find(opt) != opts.end(); }

    const ArgsContainer_t& at(const std::string& opt) const { return opts.at(opt); }

    detail::Values operator[](const std::string& opt) { return detail::Values(opts.at(opt)); }

    const std::size_t size() const noexcept { return opts.size(); }

    OptsContainer_t::const_iterator begin() const noexcept { return opts.cbegin(); }

    OptsContainer_t::const_iterator end() const noexcept { return opts.cend(); }

    const std::string& unresponded() const noexcept { return _unresponded; }

private:
    std::string _unresponded;
    OptsContainer_t opts;

private:
    void remove_first()
    {
        _unresponded.erase(0, std::min(_unresponded.size(), _unresponded.find_first_of(' ')));
        if (_unresponded.length()) _unresponded.erase(0, 1); // remove space
    }
};
}
