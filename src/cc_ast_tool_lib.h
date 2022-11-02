#ifndef CC_AST_TOOL_LIB_H_
#define CC_AST_TOOL_LIB_H_

#include "src/frontend_action.h"

#include <string>
#include <algorithm>

#include "absl/strings/string_view.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "clang/Tooling/Tooling.h"

absl::StatusOr<std::string> GetFileContents(absl::string_view path);

template <typename T>
absl::StatusOr<typename T::VisitorInfo> VisitASTOnCode(
    const absl::string_view &cc_file_content,
    std::vector<std::string> &args_as_strings,
    const std::string &cc_in = "input.cc",
    const std::string &tool_name = "clang-tool") {
  typename T::VisitorInfo visitor_info;

  auto ebegin = std::remove_if(args_as_strings.begin(), args_as_strings.end(), 
    [](auto &arg) {
      return arg == "--undefok" ||
             arg.starts_with("--cc_in=") ||
             arg.starts_with("--cc_tool=");
    });
  args_as_strings.erase(ebegin, args_as_strings.end());

  if (clang::tooling::runToolOnCodeWithArgs(
          std::make_unique<FrontendAction<T>>(visitor_info),
          cc_file_content, args_as_strings, cc_in, tool_name)) {
    return visitor_info;
  }
  return absl::Status(absl::StatusCode::kInvalidArgument, 
                      "Could not compile source file contents");
}

#endif // CC_AST_TOOL_LIB_H_