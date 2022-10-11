#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/check.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
// #include "common/file_io.h"
#include "llvm/Support/FileSystem.h"

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

  // auto status_or_cc_file_content = crubit::GetFileContents(cc_in);
  // CHECK(status_or_cc_file_content.ok());
  // std::string cc_file_content = std::move(*status_or_cc_file_content);

  // absl::StatusOr<std::string> rs_code = crubit_rs_from_cc::RsFromCc(
      // cc_file_content, cc_in,
      // std::vector<absl::string_view>(argv, argv + argc));
  // CHECK(rs_code.ok());

  // CHECK(crubit::SetFileContents(rs_out, *rs_code).ok());
  return 0;
}