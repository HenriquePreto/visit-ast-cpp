package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cast_visitor",
    srcs = ["cast_visitor.cc"],
    hdrs = ["cast_visitor.h"],
    deps = [
        "@llvm-project//clang:ast",
        "@rapidjson",
    ],
)

cc_library(
    name = "goto_visitor",
    srcs = ["goto_visitor.cc"],
    hdrs = ["goto_visitor.h"],
    deps = [
        "@llvm-project//clang:ast",
        "@rapidjson",
    ],
)

cc_library(
    name = "nobreak_visitor",
    srcs = ["nobreak_visitor.cc"],
    hdrs = ["nobreak_visitor.h"],
    deps = [
        "@llvm-project//clang:ast",
        "@rapidjson",
    ],
)