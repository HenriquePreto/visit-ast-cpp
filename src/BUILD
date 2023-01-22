package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "visit_ast",
    srcs = ["visit_ast.cc"],
    visibility = ["//visibility:public"],
    deps = [
        ":visit_ast_lib",
        "//src/visitors:cast_visitor",
        "//src/visitors:goto_visitor",
        "//src/visitors:nobreak_visitor",
        "@absl//absl/flags:flag",
        "@absl//absl/flags:parse",
        "@absl//absl/flags:usage",
        "@absl//absl/log:check",
        "@absl//absl/status:statusor",
        "@rapidjson",
    ]
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
    name = "visit_ast_lib",
    srcs = ["visit_ast_lib.cc"],
    hdrs = ["visit_ast_lib.h"],
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