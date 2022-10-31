#include "src/cc_ast_tool_lib.h"

#include "src/cast_visitor.h"
#include "src/goto_visitor.h"
#include "src/nobreak_visitor.h"

#include "absl/log/check.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"
#include "absl/status/statusor.h"
#include "rapidjson/PrettyWriter.h"

ABSL_FLAG(std::string, cc_tool, "",
          "full class name for the cc abstract syntax tree tool");
ABSL_FLAG(std::string, cc_in, "",
          "input path for the C++ source file (it may or may not be a header)");

int main(int argc, char *argv[]) {
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

  rapidjson::StringBuffer string_buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(string_buffer);
  if (cc_tool == "cast") {
    auto status_or_visitor = VisitASTOnCode<CastVisitor>(
      cc_file_content, cc_in, cc_tool);
    CHECK(status_or_visitor.ok());
    status_or_visitor->ToJson(writer);
  } else if (cc_tool == "goto") {
    auto status_or_visitor = VisitASTOnCode<GotoVisitor>(
      cc_file_content, cc_in, cc_tool);
    CHECK(status_or_visitor.ok());
    status_or_visitor->ToJson(writer);
  } else if (cc_tool == "nobreak") {
    auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
      cc_file_content, cc_in, cc_tool);
    CHECK(status_or_visitor.ok());
    status_or_visitor->ToJson(writer);
  } else {
    CHECK(false && "Not supported tool.");
  }
  std::cout << string_buffer.GetString() << std::endl;

  return 0;
}