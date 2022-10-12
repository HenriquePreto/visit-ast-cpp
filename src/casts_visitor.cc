#include "src/casts_visitor.h"

bool CastsVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  auto function_name = decl->getQualifiedNameAsString();
  auto src_range = decl->getSourceRange();
  current_function_ = 
    src_range.printToString(ctx_.getSourceManager()) + "#" + function_name;
  collector_.function_info_[current_function_] = CastsInfo();
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