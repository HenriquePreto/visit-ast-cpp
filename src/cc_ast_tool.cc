#include "absl/log/check.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/statusor.h"
#include "src/cc_ast_tool_lib.h"

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

  absl::StatusOr<std::string> status_or_cc_file_content = GetFileContents(cc_in);
  CHECK(status_or_cc_file_content.ok());
  std::string cc_file_content = std::move(*status_or_cc_file_content);

  VisitASTOnCode(cc_file_content, cc_tool);

  // TODO: missing status or of visit ast on code
  // absl::StatusOr<std::string> rs_code = crubit_rs_from_cc::RsFromCc(
      // cc_file_content, cc_in,
      // std::vector<absl::string_view>(argv, argv + argc));
  // CHECK(rs_code.ok());

  // CHECK(crubit::SetFileContents(rs_out, *rs_code).ok());
  return 0;
}