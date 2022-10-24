#include "src/cc_ast_tool_lib.h"
#include "src/cast_visitor.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace {

using ::testing::UnorderedElementsAre;

TEST(VisitASTOnCodeTest, CastNoop) {
  auto status_or_visitor = VisitASTOnCode<CastVisitor>(" ");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, CastComment) {
  auto status_or_visitor = VisitASTOnCode<CastVisitor>("// A comment in c++");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, CastEmptyFunction) {
  auto status_or_visitor = VisitASTOnCode<CastVisitor>("void f() {}");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

// TODO: fix runToolOnCode's arguments and this test!
// TEST(RsFromCcTest, ErrorOnInvalidInput) {
//   ASSERT_THAT(RsFromCc("int foo(); But this is not C++"),
//               StatusIs(absl::StatusCode::kInvalidArgument));
// }

TEST(VisitASTOnCodeTest, CastFunctionValues) {
  std::string cc_file_content = "#include <stdio.h>\n"
  "\n"
  "int GLOB = 0.3 + 3.4;\n"
  "\n"
  "namespace hello_world {\n"
  "  int bar();\n"
  "\n"
  "  void foo(int aaa, int bbb) {\n"
  "    int i = 0;\n"
  "    int j = 4;\n"
  "    int k = i + j;\n"
  "    int p = bar();\n" // 1
  "    double x = 3.3;\n"
  "    float y = 4.4f;\n"
  "    int z = x + y;\n" // 2
  "    double w = y + z;\n" // 2
  "    printf(\"\%d\", x + y);\n" // 1
  "  }\n"
  "\n"
  "  int bar() {\n"
  "    return 0.0;\n" // 1
  "  }\n"
  "\n"  
  "  namespace mynamespace {\n"
  "    int foo(int aaa, int bbb) {\n"
  "      return 0;\n" // 0
  "    }\n"
  "  }\n"
  "}\n"
  "\n"
  "int main() {\n"
  "  int i = 0;\n"
  "  int j = 4;\n"
  "  int k = i + j;\n"
  "  double x = 3.3;\n"
  "  float y = 4.4f;\n"
  "  int z = x + y;\n" // 2
  "  double w = y + z;\n" // 2
  "  hello_world::foo(10, 11);\n" // 1
  "  printf(\"\%f\", x + y);\n" // 1
  "  return 0;\n"
  "}\n";

  auto status_or_visitor = VisitASTOnCode<CastVisitor>(cc_file_content);
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 3);

  auto function_name = "src/test.cc#31:1#main";
  EXPECT_EQ(visitor.GetNumCasts(function_name), 6);
  EXPECT_EQ(visitor.GetNumVars(function_name), 7);
  ASSERT_THAT(visitor.GetCastKinds(function_name), UnorderedElementsAre(
    clang::CastKind::CK_FunctionToPointerDecay,
    clang::CastKind::CK_IntegralToFloating,
    clang::CastKind::CK_FloatingCast,
    clang::CastKind::CK_FloatingToIntegral
  ));

  function_name = "src/test.cc#20:3#hello_world::bar";
  EXPECT_EQ(visitor.GetNumCasts(function_name), 1);
  EXPECT_EQ(visitor.GetNumVars(function_name), 0);
  ASSERT_THAT(visitor.GetCastKinds(function_name), UnorderedElementsAre(
    clang::CastKind::CK_FloatingToIntegral
  ));

  function_name = "src/test.cc#8:3#hello_world::foo";
  EXPECT_EQ(visitor.GetNumCasts(function_name), 6);
  EXPECT_EQ(visitor.GetNumVars(function_name), 8);
  ASSERT_THAT(visitor.GetCastKinds(function_name), UnorderedElementsAre(
    clang::CastKind::CK_FunctionToPointerDecay,
    clang::CastKind::CK_IntegralToFloating,
    clang::CastKind::CK_FloatingCast,
    clang::CastKind::CK_FloatingToIntegral
  ));
}

}
