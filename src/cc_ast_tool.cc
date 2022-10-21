#include "src/cc_ast_tool_lib.h"

#include "src/casts_visitor.h"
#include "src/goto_visitor.h"
#include "src/nobreak_visitor.h"

#include "absl/log/check.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/statusor.h"

ABSL_FLAG(std::string, cc_tool, "",
          "full class name for the cc abstract syntax tree tool");
ABSL_FLAG(std::string, cc_in, "",
          "input path for the C++ source file (it may or may not be a header)");

int main(int argc, char* argv[]) {
  absl::SetProgramUsageMessage("Usage: --cc_tool=cast --cc_in=hello_world.cc");
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

  auto status_or_cc_file_content = GetFileContents(cc_in);
  CHECK(status_or_cc_file_content.ok());
  auto cc_file_content = std::move(*status_or_cc_file_content);

  if (cc_tool == "cast") {
    auto status_or_collector = VisitASTOnCode<CastsVisitor>(
      cc_file_content, cc_in);
    CHECK(status_or_collector.ok());
    std::cout << *status_or_collector << std::endl;
  } else if (cc_tool == "goto") {
    auto status_or_collector = VisitASTOnCode<GotoVisitor>(
      cc_file_content, cc_in);
    CHECK(status_or_collector.ok());
    std::cout << *status_or_collector << std::endl;
  } else if (cc_tool == "nobreak") {
    auto status_or_collector = VisitASTOnCode<NoBreakVisitor>(
      cc_file_content, cc_in);
    CHECK(status_or_collector.ok());
    std::cout << *status_or_collector << std::endl;
  } else {
    CHECK(false && "Not supported tool.");
  }
  // TODO: 
  // std::vector<absl::string_view>(argv, argv + argc));
  // CHECK(crubit::SetFileContents(rs_out, *rs_code).ok());
  return 0;
}