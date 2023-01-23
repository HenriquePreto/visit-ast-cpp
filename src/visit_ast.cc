#include "visit_ast_lib.h"

#include "visitors/cast_visitor.h"
#include "visitors/goto_visitor.h"
#include "visitors/nobreak_visitor.h"

#include "absl/log/check.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(std::string, ast_tool, "",
          "full class name for the cpp ast tool");
ABSL_FLAG(std::string, build_dir, "",
          "input path for the compilation database from a build directory");

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage(
    "Usage: --ast_tool=cast --build_dir=/path/to/build/llvm");
  auto args = absl::ParseCommandLine(argc, argv);

  auto ast_tool = absl::GetFlag(FLAGS_ast_tool);
  if (ast_tool.empty()) {
    std::cerr << "please specify --ast_tool" << std::endl;
    return 1;
  }
  auto build_dir = absl::GetFlag(FLAGS_build_dir);
  if (build_dir.empty()) {
    std::cerr << "please specify --build_dir" << std::endl;
    return 1;
  }
  
  auto ok = false;
  if (ast_tool == "cast") {
    ok = VisitASTOnCompilationDB<CastVisitor>(build_dir);
  } else if (ast_tool == "goto") {
    ok = VisitASTOnCompilationDB<GotoVisitor>(build_dir);
  } else if (ast_tool == "nobreak") {
    ok = VisitASTOnCompilationDB<NoBreakVisitor>(build_dir);
  }
  CHECK(ok);

  return 0;
}