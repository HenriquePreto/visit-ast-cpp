#include "src/casts_visitor.h"

bool CastsVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  auto function_name = decl->getQualifiedNameAsString();
  auto src_range = decl->getNameInfo().getSourceRange();
  current_function_ = 
    src_range.printToString(ctx_.getSourceManager()) + function_name;
  function_info_[current_function_] = CastsInfo();
  return true;
}

bool CastsVisitor::VisitCastExpr(const clang::CastExpr* expr) {
  if (current_function_ != "") {
    function_info_[current_function_].num_casts++;
  }
  return true;
}

bool CastsVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (current_function_ != "" && decl->isLocalVarDecl()) {
    function_info_[current_function_].num_vars++;
  }
  return true;
}