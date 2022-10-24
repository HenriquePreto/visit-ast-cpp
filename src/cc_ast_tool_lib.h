#ifndef CC_AST_TOOL_LIB_H_
#define CC_AST_TOOL_LIB_H_

#include "src/frontend_action.h"

#include <string>

#include "absl/strings/string_view.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "clang/Tooling/Tooling.h"

absl::StatusOr<std::string> GetFileContents(absl::string_view path);

template <typename T>
absl::StatusOr<typename T::VisitorInfo> VisitASTOnCode(
    const absl::string_view cc_file_content, 
    const std::string& cc_in = "src/test.cc",
    const std::string& tool_name = "clang-tool") {
  typename T::VisitorInfo visitor_info;
  // TODO: fix these clang -cc1 args, no error with #include directive
  std::vector<std::string> args = { 
    // "-v",
    // "-I/usr/local/opt/llvm@14/bin/../include/c++/v1",
    // "-I/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk/usr/include",
  };
  if (clang::tooling::runToolOnCodeWithArgs(
          std::make_unique<FrontendAction<T>>(visitor_info),
          cc_file_content, args, cc_in, tool_name)) {
    return visitor_info;
  }
  return absl::Status(absl::StatusCode::kInvalidArgument, 
                      "Could not compile source file contents");
}

#endif // CC_AST_TOOL_LIB_H_