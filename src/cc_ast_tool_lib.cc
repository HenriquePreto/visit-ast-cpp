#include "src/cc_ast_tool_lib.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/MemoryBuffer.h"

absl::StatusOr<std::string> GetFileContents(absl::string_view path) {
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> err_or_buffer =
    llvm::MemoryBuffer::getFileOrSTDIN(path.data(), true);
  if (std::error_code err = err_or_buffer.getError()) {
    return absl::Status(absl::StatusCode::kInternal, err.message());
  }
  return std::string((*err_or_buffer)->getBuffer());
}