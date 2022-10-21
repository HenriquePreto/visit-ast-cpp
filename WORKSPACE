load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# From https://github.com/bazelbuild/bazel-skylib/releases/tag/1.2.1
http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
        "https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz",
    ],
)

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

# From https://abseil.io/about/releases
http_archive(
    name = "absl",
    sha256 = "feea6e761ab9df667f054531b4e4d5d143eaa78f2c40dbf5ba45289142cbcc1d",
    strip_prefix = "abseil-cpp-92fdbfb301f8b301b28ab5c99e7361e775c2fb8a",
    urls = ["https://github.com/abseil/abseil-cpp/archive/92fdbfb301f8b301b28ab5c99e7361e775c2fb8a.zip"],
)

load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

# From "https://github.com/llvm/llvm-project.git"
new_git_repository(
    name = "llvm-raw",
    build_file_content = "# empty",
    commit = "fed71b04fb3cc6b1a5a21f64c26104962d816300",
    shallow_since = "1661922120 +0800",
    remote = "https://github.com/llvm/llvm-project.git",
)

load("@llvm-raw//utils/bazel:configure.bzl", 
     "llvm_configure", "llvm_disable_optional_support_deps")
llvm_configure(name = "llvm-project")
llvm_disable_optional_support_deps()

# From https://github.com/Tencent/rapidjson.git
http_archive(
    name = "rapidjson",
    urls = [
        "https://github.com/Tencent/rapidjson/archive/v1.1.0.tar.gz",
    ],
    sha256 = "bf7ced29704a1e696fbccf2a2b4ea068e7774fa37f6d7dd4039d0787f8bed98e",
    strip_prefix = "rapidjson-1.1.0",
    build_file = "//bazel:rapidjson.BUILD",
)