#!/bin/bash

SCRIPT=$(realpath "$0")
export SCRIPT_PATH=$(dirname "$SCRIPT")

export BAZEL_DIR="$SCRIPT_PATH"/..

# All available tools
export CC_TOOLS="cast goto nobreak"

# bzip2: https://github.com/libarchive/bzip2
GIT_BZIP2="https://github.com/libarchive/bzip2.git"
read -r -d '' BUILD_BZIP2 <<- EOM
  git clean -xfd;
  mkdir build;
  cd build;
  cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../;
EOM

# sqlite3: https://github.com/sqlite/sqlite
GIT_SQLITE3="https://github.com/sqlite/sqlite.git"
read -r -d '' BUILD_SQLITE3 <<- EOM
  git clean -xfd;
  mkdir build;
  cd build;
  CFLAGS="-I/usr/local/opt/sqlite3/include -I/Applications/CODE/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include -I/Applications/CODE/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.5/include -Wno-undef-prefix -Wno-nullability-completeness" CPPFLAGS="-I/usr/local/opt/sqlite3/include -I/Applications/CODE/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX11.3.sdk/usr/include -I/Applications/CODE/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.5/include -Wno-nullability-completeness" ../configure;
  bear -- make;
EOM

# z3: https://github.com/Z3Prover/z3.git
GIT_Z3="https://github.com/Z3Prover/z3.git"
read -r -d '' BUILD_Z3 <<- EOM
  git clean -xfd;
  mkdir build;
  cd build;
  CCFLAGS="-I/Applications/CODE/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.5/include" CXXFLAGS="-I/Applications/CODE/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/12.0.5/include" cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ../;
  make -j$(nproc);
EOM

# # libxml2: https://github.com/GNOME/libxml2
# PYTHON_CFLAGS="$(python3-config --cflags)"
# PYTHON_CLIBS="$(python3-config --libs)"
# GIT_LIBXML2="https://github.com/GNOME/libxml2.git"
# read -r -d '' BUILD_LIBXML2 <<- EOM
#   git clean -xfd;
#   ./autogen.sh PYTHON_CFLAGS="${PYTHON_CFLAGS}" PYTHON_LIBS="${PYTHON_CLIBS}";
#   make -j$(nproc);
# EOM

# # redis: https://github.com/redis/redis
# GIT_REDIS="https://github.com/redis/redis.git"
# read -r -d '' BUILD_REDIS <<- EOM
#   git clean -xfd;
#   make -j$(nproc);
# EOM

# # php: https://github.com/php/php-src
# GIT_PHP="https://github.com/php/php-src.git"
# read -r -d '' BUILD_PHP <<- EOM
#   git clean -xfd;
#   ./buildconf;
#   ./configure --with-iconv=$(brew --prefix libiconv) --enable-debug;
#   ninja -j$(nproc) CC=${MY_CC} CXX=${MY_CXX};
# EOM

# # blender: https://github.com/blender/blender
# GIT_BLENDER="https://github.com/blender/blender.git"
# read -r -d '' BUILD_BLENDER <<- EOM
#   rm -rf ../blender-git;
#   mkdir ../blender-git; 
#   cd ..;
#   mv blender/ blender-git/;
#   cd blender-git/blender/;
#   make -j$(nproc) clean;
#   make update;
#   make -j$(nproc) CC=${MY_CC} CXX=${MY_CXX};
# EOM

# # llvm: https://github.com/llvm/llvm-project
# GIT_LLVM="https://github.com/llvm/llvm-project.git"
# read -r -d '' BUILD_LLVM <<- EOM
#   cmake -S llvm -B build -G Ninja;
#   cd build;
#   ninja;
# EOM

# Benchmark info stored in a list of tuples
read -r -d '' BENCHMARKS_LIST <<- EOM
  $GIT_Z3,$BUILD_Z3
EOM
export BENCHMARKS_LIST
#$GIT_BZIP2,$BUILD_BZIP2#$GIT_SQLITE3,$BUILD_SQLITE3
# PATH to store all git repositories
export BENCHMARKS_PATH="$SCRIPT_PATH"/benchmarks
export OUTPUT_PATH="$SCRIPT_PATH"/out