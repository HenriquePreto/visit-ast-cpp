cc_binary(
    name = "cc_ast_tool",
    srcs = ["cc_ast_tool.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":frontend_action",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/log:check",
        "@absl//absl/status",
        "@absl//absl/status:statusor",
        "@absl//absl/strings",
        "@llvm-project//llvm:Support",
        "@llvm-project//clang:tooling",
    ],
)

cc_library(
    name = "frontend_action",
    srcs = ["frontend_action.cc"],
    hdrs = ["frontend_action.h"],
    deps = [
        ":ast_consumer",
        "@llvm-project//clang:ast",
        "@llvm-project//clang:frontend",
    ],
)

cc_library(
    name = "ast_consumer",
    srcs = ["ast_consumer.cc"],
    hdrs = ["ast_consumer.h"],
    deps = [
        ":casts_visitor",
        "@absl//absl/log:check",
        "@llvm-project//clang:ast",
    ],
)

cc_library(
    name = "casts_visitor",
    srcs = ["casts_visitor.cc"],
    hdrs = ["casts_visitor.h"],
    deps = [
        "@absl//absl/log:check",
        "@llvm-project//clang:ast",
    ],
)