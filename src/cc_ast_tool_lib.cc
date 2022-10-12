#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "clang/Tooling/Tooling.h"
#include "src/cc_ast_tool_lib.h"
#include "src/frontend_action.h"
#include "src/casts_visitor.h"

absl::StatusOr<std::string> GetFileContents(absl::string_view path) {
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> err_or_buffer =
      llvm::MemoryBuffer::getFileOrSTDIN(path.data(), true);
  if (std::error_code err = err_or_buffer.getError()) {
    return absl::Status(absl::StatusCode::kInternal, err.message());
  }

  return std::string((*err_or_buffer)->getBuffer());
}

void VisitASTOnCode(const absl::string_view cc_file_content, const absl::string_view cc_tool) {
  clang::tooling::runToolOnCode(std::make_unique<FrontendAction<CastsVisitor>>(), cc_file_content);
}