#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "RegexArgs.hpp"
#include "doctest.h"
#include <string>

using namespace std;
using namespace args;

TEST_CASE("test")
{
    Argv myopts("exe -A=arg1      -B       arg2   arg3 -DFLAG -F -P=(1,2)");

    myopts.respond<Behavior::REQUIRED>("-A", pat::equal)
        .respond<Behavior::REQUIRED>("-B", { pat::normal.basic, "+" })
        .respond<Behavior::REQUIRED>("-F", pat::flag)
        .respond<Behavior::REQUIRED>("-D", pat::suffix)
        .respond<Behavior::REQUIRED>("-P", R"(\=\((\d),(\d)\))");

    CHECK(myopts.at("-A").at(0) == "arg1");
    CHECK(myopts["-B"].size() == 2);
    CHECK(myopts["-D"][0] == "FLAG");
    CHECK(myopts["-F"].size() == 0);

    int a = myopts["-P"].as<int>(0), b;
    myopts["-P"] >> b >> b; //".as<TYPE>()" is just a peek, the stream's pointer doesn't move actutlly
    CHECK(a == 1);
    CHECK(b == 2);
}
