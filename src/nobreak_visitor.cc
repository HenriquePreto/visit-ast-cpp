#include "nobreak_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

std::ostream& operator<<(std::ostream& os, 
                         const NoBreakVisitor::Collector& collector) {
  os << "NoBreakVisitorInfo {" << std::endl;
  for (auto const& [key, val] : collector.function_info_) {
    os << "\t"
        << key
        << " ->\n"
        << val
        << "\n";
  }
  return os << "}";  
}

bool NoBreakVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto col_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    current_function_ = 
      fname + "#" + std::to_string(line_num) + ":" 
      + std::to_string(col_num)  + "#" + function_name;
    current_function_decl_ = decl->getBody();
  }
  return true;
}

bool NoBreakVisitor::VisitSwitchStmt(const clang::SwitchStmt* stmt) {
  if (!current_function_.empty() && current_function_decl_ != nullptr 
      && !hasBreaks(stmt)) {
    std::string body;
    llvm::raw_string_ostream body_stream(body);
    current_function_decl_->printPretty(body_stream, 
      nullptr, clang::PrintingPolicy(clang::LangOptions()));
    collector_.function_info_[current_function_] = body;
  }
  return true;
}

// If the case statement has just a break statement, then the BreakStmt node is child of 
// CaseStmt node. Otherwise (i.e. case statement has more than one break statement) the
// BreakStmt node is (not right) below the CaseStmt node in the AST, before the next case
// statement (i.e before the next CaseStmt node).
bool NoBreakVisitor::hasBreaks(const clang::SwitchStmt* stmt) {
  size_t num_cases = 0; 
  size_t num_breaks = 0;
  auto switch_body = stmt->getBody();
  bool found = false;
  for (clang::Stmt::const_child_iterator it = switch_body->child_begin(), end = switch_body->child_end(); it != end; it++) {
    std::string stmt_classname = it->getStmtClassName();
    // llvm::outs() << stmt_classname << "\n";
    if (stmt_classname == "CaseStmt") {
      num_cases++;
      // Case where BreakStmt is child of CaseStmt
      for (clang::Stmt::const_child_iterator it_case = it->child_begin(), end_case = it->child_end(); it_case != end_case; it_case++) {
        std::string case_child_classname = it_case->getStmtClassName(); 
        // llvm::outs() << "\t" << case_child_classname << "\n";
        if (case_child_classname == "BreakStmt") {
          found = true;
          break;
        }
      }
      // Case where BreakStmt node is below of CaseStmt node
      if (!found) {
        clang::Stmt::const_child_iterator it_prev;
        for (it_prev = it++; it != end; it_prev = it, it++) {
          std::string case_below_classname = it->getStmtClassName();
          // llvm::outs() << "\t\t" << case_below_classname << "\n";
          if (case_below_classname == "CaseStmt") {
            break;
          } else if (case_below_classname == "BreakStmt") {
            found = true;
            break;
          }
        }
        it = it_prev;
      }
      if (found) {
        num_breaks++;
        found = false;
      }
    }
  }
  // llvm::outs() << num_cases << " " << num_breaks << "\n";
  return num_cases == num_breaks;
}