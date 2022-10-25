#include "nobreak_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"
#include <algorithm>

void NoBreakVisitor::VisitorInfo::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();
  for (auto const& [key, value] : function_info_) {
    writer.String(key);
    writer.String(value);
  }
  writer.EndObject();
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
  if (!current_function_.empty() && current_function_decl_ != nullptr &&
      !IsOkSwitch(stmt)) {
    std::string body;
    llvm::raw_string_ostream body_stream(body);
    current_function_decl_->printPretty(body_stream, 
      nullptr, clang::PrintingPolicy(clang::LangOptions()));
    visitor_info_.function_info_[current_function_] = body;
  }
  return true;
}

/** 
 * If the case statement has just a break statement, then the BreakStmt node is
 * child of CaseStmt node. Otherwise (i.e. case statement has more than one
 * break statement instruction) the BreakStmt node is (not right) below the 
 * CaseStmt node in the AST, before the next case statement (i.e before the 
 * next CaseStmt node). 
 */
// TODO: compound stmt as only child inside case
// TODO: return stmts inside case are just like break stmts
// TODO: fall through case stmts are normal - case stmt works
// TODO: default are also cases
bool NoBreakVisitor::IsOkSwitch(const clang::SwitchStmt* stmt) const {
  auto num_cases = 0; 
  auto num_breaks = 0;
  auto body = stmt->getBody();
  for (auto it = body->child_begin(), end = body->child_end(); 
      it != end; it++) {
    if (it->getStmtClass() == clang::Stmt::CaseStmtClass) {
      num_cases++;
      if (HasBreakChild(it) || IsBreakBelow(it, end)) {
        num_breaks++;
      }
    }
  }
  return num_cases == num_breaks;
}

bool NoBreakVisitor::HasBreakChild(const clang::ConstStmtIterator& it) const {
  return std::any_of(it->child_begin(), it->child_end(), [](auto child) { 
      return child->getStmtClass() == clang::Stmt::BreakStmtClass ||
             child->getStmtClass() == clang::Stmt::ReturnStmtClass; 
  });
}

bool NoBreakVisitor::IsBreakBelow(
    clang::ConstStmtIterator& it, 
    const clang::ConstStmtIterator& end) const {
  auto break_found = false;
  auto it_prev = it++;
  for (; it != end; it_prev = it, it++) {
    if (it->getStmtClass() == clang::Stmt::CaseStmtClass) {
      break;
    } else if (it->getStmtClass() == clang::Stmt::BreakStmtClass ||
        it->getStmtClass() == clang::Stmt::ReturnStmtClass) {
      break_found = true;
      break;
    }
  }
  it = it_prev;
  return break_found;
}