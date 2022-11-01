#include "src/cc_ast_tool_lib.h"
#include "src/cast_visitor.h"
#include "src/goto_visitor.h"
#include "src/nobreak_visitor.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

namespace {

using ::testing::UnorderedElementsAre;

// TODO: fix runToolOnCode's arguments and this test!
// TEST(RsFromCcTest, ErrorOnInvalidInput) {
//   ASSERT_THAT(RsFromCc("int foo(); But this is not C++"),
//               StatusIs(absl::StatusCode::kInvalidArgument));
// }

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
  EXPECT_EQ(visitor.GetNumFunctions(), 1);

  auto function_id = "input.cc#1:1#f";
  EXPECT_EQ(visitor.GetNumCasts(function_id), 0);
  EXPECT_EQ(visitor.GetNumVars(function_id), 0);
  ASSERT_THAT(visitor.GetCastKinds(function_id), UnorderedElementsAre());
  EXPECT_EQ(visitor.GetFunctionSize(function_id), 1);
}

TEST(VisitASTOnCodeTest, CastFunctionValues) {
  auto cc_file_content = "#include <stdio.h>\n"
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
    "    int p = bar();\n" // 0 function to pointer decay
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
    "  int GLOB2 = 0.3 + 3.4;\n"
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
    "  hello_world::foo(10, 11);\n" // 0 function to pointer decay
    "  printf(\"\%f\", x + y);\n" // 1
    "  return 0;\n"
    "}\n";

  auto status_or_visitor = VisitASTOnCode<CastVisitor>(cc_file_content);
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 4);

  auto function_id = "input.cc#32:1#main";
  EXPECT_EQ(visitor.GetNumCasts(function_id), 5);
  EXPECT_EQ(visitor.GetNumVars(function_id), 7);
  ASSERT_THAT(visitor.GetCastKinds(function_id), UnorderedElementsAre(
    clang::CastKind::CK_IntegralToFloating,
    clang::CastKind::CK_FloatingCast,
    clang::CastKind::CK_FloatingToIntegral
  ));
  EXPECT_EQ(visitor.GetFunctionSize(function_id), 12);

  function_id = "input.cc#26:5#hello_world::mynamespace::foo";
  EXPECT_EQ(visitor.GetNumCasts(function_id), 0);
  EXPECT_EQ(visitor.GetNumVars(function_id), 0);
  ASSERT_THAT(visitor.GetCastKinds(function_id), UnorderedElementsAre());
  EXPECT_EQ(visitor.GetFunctionSize(function_id), 3);

  function_id = "input.cc#20:3#hello_world::bar";
  EXPECT_EQ(visitor.GetNumCasts(function_id), 1);
  EXPECT_EQ(visitor.GetNumVars(function_id), 0);
  ASSERT_THAT(visitor.GetCastKinds(function_id), UnorderedElementsAre(
    clang::CastKind::CK_FloatingToIntegral
  ));
  EXPECT_EQ(visitor.GetFunctionSize(function_id), 3);

  function_id = "input.cc#8:3#hello_world::foo";
  EXPECT_EQ(visitor.GetNumCasts(function_id), 5);
  EXPECT_EQ(visitor.GetNumVars(function_id), 8);
  ASSERT_THAT(visitor.GetCastKinds(function_id), UnorderedElementsAre(
    clang::CastKind::CK_IntegralToFloating,
    clang::CastKind::CK_FloatingCast,
    clang::CastKind::CK_FloatingToIntegral
  ));
  EXPECT_EQ(visitor.GetFunctionSize(function_id), 11);
}

TEST(VisitASTOnCodeTest, GotoNoop) {
  auto status_or_visitor = VisitASTOnCode<GotoVisitor>(" ");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, GotoComment) {
  auto status_or_visitor = VisitASTOnCode<GotoVisitor>("// A comment in c++");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, GotoEmptyFunction) {
  auto status_or_visitor = VisitASTOnCode<GotoVisitor>("void f() {}");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 1);

  auto function_id = "input.cc#1:1#f";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
  EXPECT_EQ(visitor.GetNumGotos(function_id), 0);
}

TEST(VisitASTOnCodeTest, GotoFunctionValues) {
  auto cc_file_content = "#include <string>\n"
    "#include <iostream>\n"
    "int foo(int z, std::string s) {\n"
    "   return s.size() + z;\n"
    "}\n"
    "\n"
    "void checkEvenOrNot(int num)\n"
    "{\n"
    "   if (num % 2 == 0)\n"
    "       // jump to even\n"
    "       goto even;\n"
    "   else\n"
    "       // jump to odd\n"
    "       goto odd;\n"
    "\n"
    "even:\n"
    "   printf(\"\%d is even\", num);\n"
    "   // return if even\n"
    "   return;\n"
    "odd:\n"
    "   printf(\"\%d is odd\", num);\n"
    "}\n"
    "\n"
    "void switch_breaks(int c) {\n"
    "   switch (c) {\n"
    "     int z;\n"
    "     case 0:\n"
    "       c++;\n"
    "       break;\n"
    "     case 1:\n"
    "       c++;\n"
    "       break;\n"
    "     case 2:\n"
    "       c++;\n"
    "       c++;\n"
    "       break;\n"
    "     case 3:\n"
    "       break;\n"
    "     case 4:\n"
    "       break;\n"
    "       c++;\n"
    "       break;\n"
    "     case 5:\n"
    "       c++;\n"
    "       break;\n"
    "       break;\n"
    "     case 6:\n"
    "       while (true) {\n"
    "         ;\n"
    "       }\n"
    "       break;\n"
    "     case 7:\n"
    "       c++;\n"
    "       c++;\n"
    "       c++;\n"
    "       break;\n"
    "   }\n"
    "}\n"
    "\n"
    "void switch_nobreaks(char c) {\n"
    "   switch (c) {\n"
    "     case 'a':\n"
    "       c++;\n"
    "     case 'e':\n"
    "     case 'i':\n"
    "       c++;\n"
    "     case 'o':\n"
    "     case 'u':\n"
    "       break;\n"
    "   }\n"
    "}\n"
    "\n"
    "int main() {\n"
    "  int x = 3;\n"
    "  int y = foo(5, \"hello_world\");\n"
    "  checkEvenOrNot(y);\n"
    "  int z = y + 5;\n"
    "  return 0;\n"
    "}";

  auto status_or_visitor = VisitASTOnCode<GotoVisitor>(cc_file_content);
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 5);

  auto function_id = "input.cc#7:1#checkEvenOrNot";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
  EXPECT_EQ(visitor.GetNumGotos(function_id), 2);
}

TEST(VisitASTOnCodeTest, GotoFunctions) {
  auto status_or_visitor = VisitASTOnCode<GotoVisitor>(
    "namespace foo {\n"
    "int f(int x) {\n"
    "   if (x % 2 == 0)\n"
    "     goto f_goto;\n"
    "   return x + 1;\n"
    "f_goto:\n"
    "   return x - 1;\n"
    "}\n"
    "}\n"
    "int g(int x) {\n"
    "   if (x % 2 == 0)\n"
    "     goto g_goto;\n"
    "   return x - 1;\n"
    "g_goto:\n"
    "   return x + 1;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 2);

  auto function_id = "input.cc#2:1#foo::f";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
  EXPECT_EQ(visitor.GetNumGotos(function_id), 1);

  function_id = "input.cc#10:1#g";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
  EXPECT_EQ(visitor.GetNumGotos(function_id), 1);
}

TEST(VisitASTOnCodeTest, NoBreakNoop) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(" ");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakComment) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "// A comment in c++"
  );
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakEmptyFunction) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>("void f() {}");
  EXPECT_TRUE(status_or_visitor.ok());
  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakFunctionValues) {
  auto cc_file_content = "#include <string>\n"
    "#include <iostream>\n"
    "int foo(int z, std::string s) {\n"
    "   return s.size() + z;\n"
    "}\n"
    "\n"
    "void checkEvenOrNot(int num)\n"
    "{\n"
    "   if (num % 2 == 0)\n"
    "       // jump to even\n"
    "       goto even;\n"
    "   else\n"
    "       // jump to odd\n"
    "       goto odd;\n"
    "\n"
    "even:\n"
    "   printf(\"\%d is even\", num);\n"
    "   // return if even\n"
    "   return;\n"
    "odd:\n"
    "   printf(\"\%d is odd\", num);\n"
    "}\n"
    "\n"
    "void switch_breaks(int c) {\n"
    "   switch (c) {\n"
    "     int z;\n"
    "     case 0:\n"
    "       c++;\n"
    "       break;\n"
    "     case 1:\n"
    "       c++;\n"
    "       break;\n"
    "     case 2:\n"
    "       c++;\n"
    "       c++;\n"
    "       break;\n"
    "     case 3:\n"
    "       break;\n"
    "     case 4:\n"
    "       break;\n"
    "       c++;\n"
    "       break;\n"
    "     case 5:\n"
    "       c++;\n"
    "       break;\n"
    "       break;\n"
    "     case 6:\n"
    "       while (true) {\n"
    "         ;\n"
    "       }\n"
    "       break;\n"
    "     case 7:\n"
    "       c++;\n"
    "       c++;\n"
    "       c++;\n"
    "       break;\n"
    "   }\n"
    "}\n"
    "\n"
    "void switch_nobreaks(char c) {\n"
    "   switch (c) {\n"
    "     case 'a':\n"
    "       c++;\n"
    "     case 'e':\n"
    "       break;\n"
    "     case 'i':\n"
    "       c++;\n"
    "     case 'o':\n"
    "       c++;\n"
    "     case 'u':\n"
    "       c++;\n"
    "       break;\n"
    "   }\n"
    "}\n"
    "\n"
    "int main() {\n"
    "  int x = 3;\n"
    "  int y = foo(5, \"hello_world\");\n"
    "  checkEvenOrNot(y);\n"
    "  int z = y + 5;\n"
    "  return 0;\n"
    "}";

  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(cc_file_content);
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 1);

  auto function_id = "input.cc#60:1#switch_nobreaks";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
} 

TEST(VisitASTOnCodeTest, NoBreakReturnChild) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       return x + 1;\n"
    "     case 2:\n"
    "       return x - 1;\n"
    "     case 3:\n"
    "       return x + x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakReturnBelow) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "       return x + 1;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       return x - 1;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "       return x + x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakDefault) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "       return x + 1;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       return x - 1;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "       return x + x;\n"
    "     default:\n"
    "       return x;\n"
    "   }\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakDefaultNoStop) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "       return x + 1;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       return x - 1;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "       return x + x;\n"
    "     default:\n"
    "       x = x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakIgnoreLast) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "       return x + 1;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       return x - 1;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "       return x + x;\n"
    "     case 4:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakFallThrough) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "     case 2:\n"
    "     case 3:\n"
    "       return x + 1;\n"
    "     case 4:\n"
    "     case 5:\n"
    "     case 6:\n"
    "       return x - 1;\n"
    "     case 7:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakFallThroughAll) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "     case 2:\n"
    "     case 3:\n"
    "     case 4:\n"
    "     case 5:\n"
    "     case 6:\n"
    "     case 7:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakComplexFallThrough) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x++;\n"
    "     case 2:\n"
    "     case 3:\n"
    "     case 4:\n"
    "     case 5:\n"
    "     case 6:\n"
    "     case 7:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 1);

  auto function_id = "input.cc#1:1#f";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
}

TEST(VisitASTOnCodeTest, NoBreakNoFallThrough) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       break;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "     case 4:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 1);

  auto function_id = "input.cc#1:1#f";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
}

TEST(VisitASTOnCodeTest, NoBreakCompoundStmtOK) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "      {\n"
    "        x++;\n"
    "        x++;\n"
    "        break;\n"
    "      }\n"
    "     case 2:\n"
    "      {\n"
    "        x++;\n"
    "        break;\n"
    "      }\n"
    "     case 3:\n"
    "      {\n"
    "        break;\n"
    "      }\n"
    "     case 4:\n"
    "      {\n"
    "        break;\n"
    "      }\n"
    "     case 5:\n"
    "     case 6:\n"
    "     case 7:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

TEST(VisitASTOnCodeTest, NoBreakCompoundStmtNOK) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "      {\n"
    "        x++;\n"
    "        x++;\n"
    "      }\n"
    "     case 2:\n"
    "      {\n"
    "        x++;\n"
    "        break;\n"
    "      }\n"
    "     case 3:\n"
    "     case 4:\n"
    "      {\n"
    "        break;\n"
    "      }\n"
    "     case 5:\n"
    "     case 6:\n"
    "      {\n"
    "      }\n"
    "     case 7:\n"
    "       x = x * x;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
    "int g(int x) {\n"
    "   while (1) {\n"
    "     switch(x) {\n"
    "       case 1:\n"
    "         x++;\n"
    "       case 2:\n"
    "         x++;\n"
    "         break;\n"
    "       case 3:\n"
    "         x++;\n"
    "       case 4:\n"
    "         x++;\n"
    "         return x;\n"
    "       case 5:\n"
    "         return x;\n"
    "       default:\n"
    "         return -1;\n"
    "   }\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 2);

  auto function_id = "input.cc#1:1#f";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));

  function_id = "input.cc#27:1#g";
  EXPECT_TRUE(visitor.ContainsFunction(function_id));
}

TEST(VisitASTOnCodeTest, NoBreakThrow) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "       x = x + 1;\n"
    "       break;\n"
    "     case 2:\n"
    "       x = x - 1;\n"
    "       break;\n"
    "     case 3:\n"
    "       x = x + x;\n"
    "       throw 1;\n"
    "     case 4:\n"
    "       x = x * x;\n"
    "       throw \"error\";\n"
    "     case 5:\n"
    "       x = x - 1;\n"
    "   }\n"
    "   return x;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

// TEST(VisitASTOnCodeTest, NoBreakFallUntilEnd) {
//   auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
//     "int f(int x) {\n"
//     "   switch(x) {\n"
//     "     case 1:\n"
//     "     case 2:\n"
//     "       ;    \n"
//     "     case 3:\n"
//     "     case 4:\n"
//     "     case 5:\n"
//     "       ;    \n"
//     "   }\n"
//     "   return x + 1;\n"
//     "}\n"
//   );
//   EXPECT_TRUE(status_or_visitor.ok());

//   auto visitor = std::move(*status_or_visitor);
//   EXPECT_EQ(visitor.GetNumFunctions(), 0);
// }

TEST(VisitASTOnCodeTest, NoBreakFallThroughCompoundStmt) {
  auto status_or_visitor = VisitASTOnCode<NoBreakVisitor>(
    "int f(int x) {\n"
    "   switch(x) {\n"
    "     case 1:\n"
    "     case 2:\n"
    "     {       \n"
    "       x++;  \n"
    "       break;\n"
    "     }       \n"
    "     case 3:\n"
    "     case 4:\n"
    "     case 5:\n"
    "     {       \n"
    "       x--;  \n"
    "       break;\n"
    "     }       \n" 
    "     case 6:\n"
    "     {       \n"
    "       x--;  \n"
    "       x--;  \n"
    "       x--;  \n"
    "       break;\n"
    "     }       \n" 
    "     case 7:\n"
    "     case 8:\n"
    "     case 9:\n"
    "     case 10:\n"
    "       ;    \n"
    "   }\n"
    "   return x + 1;\n"
    "}\n"
  );
  EXPECT_TRUE(status_or_visitor.ok());

  auto visitor = std::move(*status_or_visitor);
  EXPECT_EQ(visitor.GetNumFunctions(), 0);
}

}
