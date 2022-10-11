#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/FileSystem.h"
#include "clang/Tooling/Tooling.h"
#include "src/frontend_action.h"

ABSL_FLAG(std::string, cc_tool, "",
          "full class name for the cc abstract syntax tree tool");
ABSL_FLAG(std::string, cc_in, "",
          "input path for the C++ source file (it may or may not be a header)");

int main(int argc, char* argv[]) {
  auto args = absl::ParseCommandLine(argc, argv);

  auto cc_tool = absl::GetFlag(FLAGS_cc_tool);
  if (cc_tool.empty()) {
    std::cerr << "please specify --cc_tool" << std::endl;
    return 1;
  }
  auto cc_in = absl::GetFlag(FLAGS_cc_in);
  if (cc_in.empty()) {
    std::cerr << "please specify --cc_in" << std::endl;
    return 1;
  }

  absl::StatusOr<std::string> status_or_cc_file_content;
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> err_or_buffer =
      llvm::MemoryBuffer::getFileOrSTDIN(cc_in.data(), true);
  if (std::error_code err = err_or_buffer.getError()) {
    status_or_cc_file_content = absl::Status(absl::StatusCode::kInternal, err.message());
  } else {
    status_or_cc_file_content = std::string((*err_or_buffer)->getBuffer());
  }
  CHECK(status_or_cc_file_content.ok());
  std::string cc_file_content = std::move(*status_or_cc_file_content);

  clang::tooling::runToolOnCode(std::make_unique<FrontendAction>(), cc_file_content);

  // absl::StatusOr<std::string> rs_code = crubit_rs_from_cc::RsFromCc(
      // cc_file_content, cc_in,
      // std::vector<absl::string_view>(argv, argv + argc));
  // CHECK(rs_code.ok());

  // CHECK(crubit::SetFileContents(rs_out, *rs_code).ok());
  return 0;
}