# MIFSA-BASE

The base library in [mifsa](https://github.com/lujuntuan/mifsa).

## Features:

- Base:
  - Application:
  - Singleton:
  - Log:
  - Elapsed:
  - Thread:
  - Semaphore:
  - Queue:
  - Timer:
  - Pool:
  - Plugin:
  - Variant:
- Moudle:
  - Client:
  - Interface:
  - Platform:
  - Provider:
  - Server:
- Utils:
  - Dir:
  - Host:
  - String:
  - System:
  - Time:

## Requirements:

Compiler: this makes heavy use of C++11 and requires a recent compiler and STL. The following compilers are known to compile the test programs:

- msvc2015+ on Windows7
- clang-3.4+ on Ubuntu-14.04
- g++-4.9+ on Ubuntu-14.04
- qcc7.0.0+ on Qnx7.0.0
- ndk r12+ on Android8

This repository includes the following connectors:

- [dylib](https://github.com/martin-olivier/dylib) (header only)
- [ghc](https://github.com/gulrak/filesystem) (header only)
- [popl](https://github.com/badaix/popl) (header only)

## How to build:

```cmake
cmake -B build
cmake --build build --target install
```

Optional:

- -DMIFSA_BUILD_EXAMPLES: 

  whether to compile the examples, default is on.

- -DMIFSA_BUILD_TESTS :

  whether to compile the tests, default is on.

Examples:

```shell
cmake -B build \
	-DCMAKE_INSTALL_PREFIX=build/install \
	-DMIFSA_BUILD_EXAMPLES=ON \
	-DMIFSA_BUILD_TESTS=OFF
cmake --build build --target install
```

## How to use:

In CMakeLists.txt:

```cmake
...
find_package(mifsa_base REQUIRED)
target_link_libraries(
    [TARGET]
    mifsa_base
    )
...
```

In cpp code:

```c++
#include <mifsa/base/application.h>

using namespace Mifsa;

class DemoApplication : public Application, public Queue {
public:
    DemoApplication(int argc, char** argv)
        : Application(argc, argv)
        , Queue(0)
    {
    }
    ~DemoApplication()
    {
    }
    virtual int exec(int flag = CHECK_SINGLETON | CHECK_TERMINATE) override
    {
        this->parserFlag(flag);
        return Queue::run();
    }
    virtual void exit(int exitCode = 0) override
    {
        Queue::quit(exitCode);
    }
    virtual void eventChanged(const std::shared_ptr<Event>& event) override
    {
        //TODO
    }
    virtual void begin() override
    {
        LOG_DEBUG("begin");
    }
    virtual void end() override
    {
        LOG_DEBUG("end");
    }
}

int main(int argc, char* argv[])
{
    DemoApplication app(argc, argv);
    return app.exec();
}
```

## Copyright:

Juntuan.Lu, 2020-2030, All rights reserved.
