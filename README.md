# RegexArgs
## What is this
RegexArgs is a header-only libaray used to assist CLI program in arguments parsing, developed based on C++20.  
Its most important features are the regular expression-based interface and some pre-built auxiliary tools, which can make your parameter semantics more explicit.

## Overview
Here is some code snippets that demonstrates the features of this library.

### Parse options and their arguments
```cpp
args::Argv myargs(argc, argv);
myargs.respond("-A", args::pat::equal, args::resp::print)
    .respond<args::Behavior::REQUIRED>("-B", { args::pat::normal.basic, "{1}" })
    .respond("-C", R"(\=\((\d),(\d)\))", [](args::resp::RespContext ctx) {
        cout << "-C.size() == " << ctx.args.size() << endl;
    });

```
As you can see, the main method is "`respond`". It controls how to parse, what to parse, and the action on success.  
The method can specify the form of the arguments, which is also indicated by the name of the library, it supports regular expressions.

### View and Get arguments
```cpp
cout << "-B[0]: " << myargs["-B"][0] << endl;

int a1 = 0, a2 = myargs["-C"].as<int>(1);
myargs["-C"] >> a1;
cout << "-C[0]: " << a1 << " -C[1]:" << a2 << endl;
cout << endl;

for (const auto& e : myargs) {
    cout << e.first << ": ";
    for (const auto& ee : myargs[e.first])
        cout << ee << " ";
    cout << endl;
}
```
The `operator[]` allows random access to the arguments of an option as if it were a vector. But it also allows additional further conversion support, the basic types are already fully supported. Of course there is also stream-like inputs: `operator>>`

### Check for missed
```
cout << "unknown args: " << myargs.unresponded() << endl;
```
This is useful to indicate to the user that the input was incorrect.

## Usage
### Build
This is a header-only library, no compilation is required, just include RegexArgs.hpp like iostream.  
(ps: You can find the version of the single header file from the Release page or build by yourself. __Otherwise you need to move the include folder to your directory.__)

#### Build the single header file yourself
After executing the following commands, you can find it in the `include` directory.
```bash
mkdir build && cd build
cmake .. -DARGS_BUILD_SINGLE_HEADER=ON
cmake --build .
```

#### Configure in your project with CMake
Add this to your CMakeLists.txt:
```cmake
add_subdirectory(RegexArgs) # or your path to this library
target_link_libraries(YOUR_PTOJECT PRIVATE RegexArgs) # change to your project name
```
If you want to use boost's regex library, add also:
```cmake
add_subdirectory(regex) # or your path to the boost regex library
target_link_libraries(YOUR_PROJECT PRIVATE boost_regex)
```
then enable the option when build your project in cmake: `-DARGS_USE_BOOST_REGEX=ON`


#### Compile the unit tests
We use [doctest](https://github.com/doctest/doctest) as the unit testing framework, which is a header-only library. It will automatically download if it does not exist in the `test` directory. Now, execute these commands:
```bash
# We are now in the root directory of the repository.
mkdir build && cd build
# If you want to use boost's regex library, add: -DARGS_USE_BOOST_REGEX=ON
# and make sure there is a regex library in the environment or the test directory.
cmake .. -DARGS_BUILD_TESTS=ON 
cmake --build .
# The test will be executed automatically.
```

### Class & Method
#### Class `args::Argv`
This is the main class, which is responsible for receiving, parsing and obtaining. It can be constructed in two ways: from a command, or from two arguments to the `main` function.
```cpp
using namespace args;
//1.
Argv myargs1("-A arg1", false);
//2.
Args myargs2(argc, argv/*, true*/);
```
The last parameter controls whether to skip the first parameter, because usually the first parameter does not make much sense.

#### Class `args::pat::Pattern`
It consists of only two strings, representing the pattern and the times of matches. Both patterns and times are expressed using the [ECMAScript regular expression grammar](https://en.cppreference.com/w/cpp/regex/ecmascript), and they are separated only to reduce unnecessary parsing.  
Constructing it is simple:
```cpp
args::pat::Pattern p1 = "\\w";
args::pat::Pattern p2 = {"\\w", "+"};
```
No need to worry about whether you should wrap the pattern with "(?:)", this happens automatically. When constructed from just a string, the times will be empty and the pattern will match only once, as the grammar specifies.

The library provides some preset pattern for use, see them in `args::pat` for details.

#### Aliaes `args::resp::Response_t`
```cpp
using Response_t = std::function<void(args::resp::RespContext)>;
```

#### Class `args::resp::RespContext`
```cpp
using ArgsContainer_t = std::vector<std::string>;

struct RespContext 
{
    args::Argv& opts;
    const std::string& opt;
    args::ArgsContainer_t& args;
};
```

#### Method `args::Argv::respond`

It is used to parse the arguments just passed in when constructing. It has three behaviors:
* `args::Behavior::DEFAULT`: When parsing fails, the response is also executed.
* `args::Behavior::OPTIONAL`: When parsing fails, do nothing.
* `args::Behavior::REQUIRED`: When parsing fails, a std::invalid_argument exception is thrown.

When calling, it is passed in as a template parameter. The default behavior is `args::Behavior::OPTIONAL`.

The method parameters are: 
* `auto&& opt`: The option string to parse.
* `const pat::Pattern& pattern`: arguments pattern.
* `const resp::Response_t& response`: action after responding that can be a callable object.
```cpp
try {
    myargs.respond<args::Behavior::REQUIRED>("-A", "\\w", args::resp::print);
} catch (std::exception e) {
    cout << e.what() << endl;
}

```
#### Method `args::Argv::reset`
Reset the parsing object for the current instance.
```cpp
args::Argv myargs("exe -c -d");
myargs.respond("-c", args::pat::flag);
cout << myargs.size() << endl; // 1

myargs.reset("hello world");
cout << myargs.size() << endl // 0
myargs.reset(argc, argv);
```

#### Method `args::Argv::operator[]`
Returns a wrapper to a const vector that provides some convenience methods. You can only get an instance of this wrapper from this method.
```cpp
myargs["-A"][0] = "1" //Error
```

#### Method `args::detail::Values::as`
A method in the wrapper obtained from `args::Argv::operator[]` that converts an argument to a primitive type. The required parameter is an index or iterator specifying the parameter string you want to convert, the default is 0. Pass in the type you want to convert to through the template parameter.  
```cpp
int a = myargs["-A"].as<int>(0);
float b = myargs["-B"].as<float>(1);
```

#### Method `args::detail::Values::operator>>`
A stream accessor that gets you an object of the type you want, just like using cin.  
__Note that the `as` method just peeks at the contents without moving the stream pointer.__
```cpp
int a = myargs["-A"].as<int>(0), b = 0, c = 0;
cout << a << endl; // assume that 1
myargs["-A"] >> b >> c;
cout << b << endl; // is also 1
cout << c << endl; // is 2
```

#### Method `args::Argv::unresponded`
You can get the unsuccessful response content from this, which is a string.

#### Others
The other methods are as simple as you can see.

## License
The project is licensed under the MIT license. Please respect the work of others.

## Issues
If you encounter any problems please open an issuse and explain there, I will check and reply regularly.
