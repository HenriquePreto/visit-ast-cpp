#ifndef CC_AST_TOOL_LIB_H_
#define CC_AST_TOOL_LIB_H_

#include <string>
#include "absl/strings/string_view.h"
#include "absl/status/statusor.h"

absl::StatusOr<std::string> GetFileContents(absl::string_view path);
void VisitASTOnCode(const absl::string_view cc_file_content, const absl::string_view cc_tool);

#endif // CC_AST_TOOL_LIB_H_