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
bool NoBreakVisitor::IsOkSwitch(const clang::SwitchStmt* stmt) const {
  auto num_cases = 0; 
  auto num_breaks = 0;
  auto body = stmt->getBody();
  for (auto it = body->child_begin(), end = body->child_end(); 
      it != end; it++) {
    if (it->getStmtClass() == clang::Stmt::CaseStmtClass) {
      num_cases++;
      if (HasBreakChild(it) || IsBreakBelow(it, end) || 
          IsFallThroughCase(it) || IsCompoundStmt(it)) {
        num_breaks++;
      }
    }
  }
  return num_cases == num_breaks;
}

bool IsBreakStmt(const clang::Stmt* stmt) {
  auto stmt_class = stmt->getStmtClass();
  return stmt_class == clang::Stmt::BreakStmtClass ||
         stmt_class == clang::Stmt::ReturnStmtClass ||
         stmt_class == clang::Stmt::CXXThrowExprClass;
        // TODO: exit function, resolve recovery expression
        // (stmt_class == clang::Stmt::CallExprClass && );
}

bool NoBreakVisitor::HasBreakChild(const clang::ConstStmtIterator& it) const {
  return std::any_of(it->child_begin(), it->child_end(), [](auto child) { 
      return IsBreakStmt(child);
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
    } else if (IsBreakStmt(*it)) {
      break_found = true;
      break;
    }
  }
  break_found = break_found || it == end;
  it = it_prev;
  return break_found;
}

bool NoBreakVisitor::IsFallThroughCase(clang::ConstStmtIterator it) const {
  while (AssignIfHasCaseChild(it)) {
    if (HasBreakChild(it)) {
      return true;
    }
  }
  return false;
}

bool NoBreakVisitor::AssignIfHasCaseChild(clang::ConstStmtIterator& it) const {
  auto it_case_child = std::find_if(it->child_begin(), it->child_end(), 
    [](auto child) {
      return child->getStmtClass() == clang::Stmt::CaseStmtClass;
    });
  if (it_case_child != it->child_end()) {
    it = it_case_child;
    return true;
  }
  return false;
}

bool NoBreakVisitor::IsCompoundStmt(const clang::ConstStmtIterator& it) const {
  auto it_compound_stmt = std::find_if(it->child_begin(), it->child_end(), 
    [](auto child) {
      return child->getStmtClass() == clang::Stmt::CompoundStmtClass;
    });
  if (it_compound_stmt == it->child_end())
    return false;
  return HasBreakChild(it_compound_stmt);
}