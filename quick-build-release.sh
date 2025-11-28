#! /bin/bash

runFlag=false
releaseFlag=false
cleanFlag=false
buildGui=true
buildTests=true
runTests=false
debugCxxFlag=false   # <-- yeni: -g ile -g3 verelim

if [ $# -eq 0 ]; then
    echo "Just compiling."
fi

for var in "$@"
do
        if [ "$var" = "-r" ];
        then
            echo "Running after Compiling."
            runFlag=true

        # ARTIK -R = Release, -g = derleyiciye -g3
        elif [ "$var" = "-R" ];
        then
            echo "Release build selected."
            releaseFlag=true

        elif [ "$var" = "-g" ];
        then
            echo "Compiler debug flags (-g3) enabled."
            debugCxxFlag=true

        elif [ "$var" = "-c" ];
        then
            echo "Clean applied."
            cleanFlag=true

        elif [ "$var" = "--nogui" ];
        then
            echo "GUI build disabled (core library/tests only)."
            buildGui=false

        elif [ "$var" = "--no-tests" ];
        then
            echo "Tests will not be built."
            buildTests=false

        elif [ "$var" = "--run-tests" ];
        then
            echo "Tests will run after the build."
            runTests=true
        fi
done

# Basic preflight check so users get a clear message when Qt build tools are missing
if [ "$buildGui" = true ] && ! command -v qmake >/dev/null 2>&1 && ! command -v qmake6 >/dev/null 2>&1; then
    echo "Qt build tools (qmake/qmake6) are not installed. Please install Qt development packages first or pass --nogui." >&2
    exit 1
fi

if [ "$cleanFlag" = true ]
then
    rm -rf cmake-build-cpp
fi

mkdir -p cmake-build-cpp
cd cmake-build-cpp || exit

buildType="Debug"
if [ "$releaseFlag" = true ]; then
    buildType="Release"
fi

# CMake çağrısı: debugCxxFlag=true ise -DCMAKE_CXX_FLAGS="-g3" ekle
if [ "$debugCxxFlag" = true ]; then
    cmake .. \
        -DCMAKE_BUILD_TYPE="${buildType}" \
        -DBARBER_BUILD_GUI=$([ "$buildGui" = true ] && echo ON || echo OFF) \
        -DBARBER_BUILD_TESTS=$([ "$buildTests" = true ] && echo ON || echo OFF) \
        -DCMAKE_CXX_FLAGS="-g3"
else
    cmake .. \
        -DCMAKE_BUILD_TYPE="${buildType}" \
        -DBARBER_BUILD_GUI=$([ "$buildGui" = true ] && echo ON || echo OFF) \
        -DBARBER_BUILD_TESTS=$([ "$buildTests" = true ] && echo ON || echo OFF)
fi

make -j "$(nproc)"

if [ "$runTests" = true ]
then
    ctest --output-on-failure
fi
cd ..  || exit

if [ "$runFlag" = true ]
then
    # exe aslında project altına düşüyor
    ./cmake-build-cpp/project/BarberShopApp
fi
