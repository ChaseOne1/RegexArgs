#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "RegexArgs.hpp"
#include "doctest.h"
#include <regex>
#include <string>

using namespace std;
using namespace args;

TEST_CASE("separator")
{
    string space = "   -   =foo      ";
    const string& equal = pat::equal;
    regex e(equal);
    smatch m;

    REQUIRE_UNARY(regex_search(space, m, e));
    CHECK_UNARY(m[1].str() == "foo");
}

TEST_CASE("normal")
{
    string args = "exe arg1          arg2 arg3";
    string args1 = "exe -OPT";
    regex e(pat::normal.basic);
    smatch m;

    REQUIRE_UNARY(regex_search(args, m, e));
    CHECK_UNARY(m[1].str() == "arg1");

    CHECK_UNARY_FALSE(regex_search(args1, m, e));
}
