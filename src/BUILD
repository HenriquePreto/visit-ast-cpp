cc_binary(
    name = "cc_ast_tool",
    srcs = ["cc_ast_tool.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":cc_ast_tool_lib",
        ":cast_visitor",
        ":goto_visitor",
        ":nobreak_visitor",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
        "@absl//absl/log:check",
        "@absl//absl/status:statusor",
        "@rapidjson",
    ],
)

cc_library(
    name = "frontend_action",
    hdrs = ["frontend_action.h"],
    deps = [
        ":ast_consumer",
        "@llvm-project//clang:ast",
        "@llvm-project//clang:frontend",
    ],
)

cc_library(
    name = "ast_consumer",
    hdrs = ["ast_consumer.h"],
    deps = [
        "@llvm-project//clang:ast",
    ],
)

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

cc_library(
    name = "cc_ast_tool_lib",
    srcs = ["cc_ast_tool_lib.cc"],
    hdrs = ["cc_ast_tool_lib.h"],
    deps = [
        ":frontend_action",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
        "@llvm-project//clang:ast",
        "@llvm-project//llvm:Support",
        "@llvm-project//clang:tooling",
    ],
)

cc_test(
    name = "cc_ast_tool_lib_test",
    srcs = ["cc_ast_tool_lib_test.cc"],
    deps = [
        ":cc_ast_tool_lib",
        ":cast_visitor",
        "@com_google_googletest//:gtest_main",
    ],
)