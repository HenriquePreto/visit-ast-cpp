#include "nobreak_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

std::ostream& operator<<(std::ostream& os, const NoBreakVisitor::Collector& collector) {
  os << "CastVisitorInfo {" << std::endl;
  for (auto const& [key, val] : collector.function_info_) {
    os << "\t"
        << key
        << " ->\n"
        << val
        << std::endl;
  }
  os << "}";  
  return os;
}

bool NoBreakVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto col_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    current_function_ = 
      fname + "#" + std::to_string(line_num) + ":" + std::to_string(col_num)  + "#" + function_name;
    current_function_decl_ = decl->getBody();
  }
  return true;
}

bool NoBreakVisitor::VisitSwitchStmt(const clang::SwitchStmt* stmt) {
  if (current_function_ != "" && current_function_decl_ != nullptr && !hasBreaks(stmt)) {
    std::string body;
    llvm::raw_string_ostream body_stream(body);
    current_function_decl_->printPretty(body_stream, nullptr, clang::PrintingPolicy(clang::LangOptions()));
    collector_.function_info_[current_function_] = body;
  }
  return true;
}

bool NoBreakVisitor::hasBreaks(const clang::SwitchStmt* stmt) {
  size_t num_cases = 0; 
  size_t num_breaks = 0;
  for (auto switch_case = stmt->getSwitchCaseList(); switch_case; switch_case = switch_case->getNextSwitchCase()) {
    num_cases++;
    std::cout << "-----" << std::endl;
    for (clang::Stmt::const_child_iterator it = switch_case->child_begin(), end = switch_case->child_end(); it != end; it++) {
      it->dump();
    }
    // if () {
    //   num_breaks++;
    // }
  }
  return num_cases == num_breaks;
}