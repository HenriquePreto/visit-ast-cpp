#ifndef CC_AST_TOOL_LIB_H_
#define CC_AST_TOOL_LIB_H_

#include "frontend_action.h"

#include <string>
#include <memory>

#include "absl/strings/string_view.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"

template <typename T>
absl::StatusOr<std::unique_ptr<typename T::Info>> VisitASTOnCode(
    const absl::string_view &cc_file_content,
    const std::string &cc_in = "input.cc") {
  typename T::Info *info = new typename T::Info();;
  if (clang::tooling::runToolOnCode(
        std::make_unique<FrontendAction<T>>(info), cc_file_content, cc_in)) {
    return std::unique_ptr<typename T::Info>(info);
  }
  return absl::Status(absl::StatusCode::kInvalidArgument, 
                      "Could not compile source file contents");
}

template <typename T>
bool VisitASTOnCompilationDB(const std::string &build_dir) {
  std::string error_message = "Invalid build directory: " + build_dir;
  auto cdb = clang::tooling::CompilationDatabase::loadFromDirectory(
    build_dir, error_message);
  if (!cdb)
    return false;
  clang::tooling::ClangTool Tool(*cdb, cdb->getAllFiles());
  auto factory = clang::tooling::newFrontendActionFactory<FrontendAction<T>>();
  return Tool.run(factory.get()) == 0;
}

#endif // CC_AST_TOOL_LIB_H_