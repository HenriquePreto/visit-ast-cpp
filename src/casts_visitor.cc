#include "src/casts_visitor.h"
#include "clang/Basic/SourceManager.h"

bool CastsVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto col_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();

    current_function_ = 
      fname + "#" + std::to_string(line_num) + ":" + std::to_string(col_num)  + "#" + function_name;
    collector_.function_info_[current_function_] = CastsInfo();
  }
  return true;
}

bool CastsVisitor::VisitCastExpr(const clang::CastExpr* expr) {
  std::string cast_kind = expr->getCastKindName();
  if (current_function_ != "" && cast_kind != "LValueToRValue") {
    collector_.function_info_[current_function_].num_casts++;
  }
  return true;
}

bool CastsVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (current_function_ != "" && decl->isLocalVarDecl()) {
    collector_.function_info_[current_function_].num_vars++;
  }
  return true;
}