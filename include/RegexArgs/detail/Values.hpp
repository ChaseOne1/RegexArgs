#include "../fwd.hpp"
#include "../utility.hpp"
#include <charconv>

namespace args
{
namespace detail
{
    class Values
    {
        friend class args::Argv;

        const ArgsContainer_t& args;
        std::string args_string;

    private:
        Values(const ArgsContainer_t& _args)
            : args(_args)
            , args_string(internal::splice(args.size(), args, false))
        { }

    public:
        Values(const Values&) = delete;
        Values(Values&&) = delete;
        ~Values() = default;

        template <typename T>
        T as(ArgsContainer_t::size_type index = 0) const
        {
            T val {};
            std::from_chars(args[index].data(), args[index].data() + args[index].length(), val);
            return val;
        }

        ArgsContainer_t::const_reference front() const { return args.front(); }

        ArgsContainer_t::const_reference back() const { return args.back(); }

        const ArgsContainer_t::value_type* data() const noexcept { return args.data(); }

        ArgsContainer_t::const_reference operator[](ArgsContainer_t::size_type index) const { return args[index]; }

        ArgsContainer_t::const_reference at(ArgsContainer_t::size_type index) const { return args.at(index); }

        bool empty() const noexcept { return args.empty(); }

        ArgsContainer_t::size_type size() const noexcept { return args.size(); }

        ArgsContainer_t::size_type max_size() const noexcept { return args.max_size(); }

        ArgsContainer_t::const_iterator begin() const noexcept { return args.begin(); }
        ArgsContainer_t::const_iterator cbegin() const noexcept { return args.cbegin(); }

        ArgsContainer_t::const_iterator end() const noexcept { return args.end(); }
        ArgsContainer_t::const_iterator cend() const noexcept { return args.cend(); }

        ArgsContainer_t::const_reverse_iterator rbegin() const noexcept { return args.rbegin(); }
        ArgsContainer_t::const_reverse_iterator crbegin() const noexcept { return args.crbegin(); }

        ArgsContainer_t::const_reverse_iterator rend() const noexcept { return args.rend(); }
        ArgsContainer_t::const_reverse_iterator crend() const noexcept { return args.crend(); }

        Values& operator>>(auto& value)
        {
            auto& ss = internal::StringStream::instance(args_string);
            ss >> value >> args_string;

            return *this;
        }
    };
}
}
