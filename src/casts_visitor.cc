#include "src/casts_visitor.h"
#include "clang/Basic/SourceManager.h"

std::ostream& operator<<(std::ostream& os, 
                         const CastsVisitor::CastsInfo& casts_info) {
  os << "(num_casts=" 
     << casts_info.num_casts 
     << ", num_vars=" 
     << casts_info.num_vars 
     << ", freq="
     << static_cast<double>(casts_info.num_casts) / casts_info.num_vars;
  return os << ')';
}

std::ostream& operator<<(std::ostream& os, 
                         const CastsVisitor::VisitorInfo& visitor_info) {
  os << "CastVisitorInfo {" << std::endl;
  for (auto const& [key, val] : visitor_info.function_info_) {
    os << '\t'
       << key
       << " -> "
       << val
       << ",\n";
  }
  return os << '}'; 
}

bool CastsVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto column_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    current_function_ = 
      fname + "#" + std::to_string(line_num) + 
      ":" + std::to_string(column_num)  + "#" + function_name;
  }
  return true;
}

bool CastsVisitor::VisitCastExpr(const clang::CastExpr* expr) {
  auto cast_kind = expr->getCastKind();
  if (current_function_.empty() && 
      cast_kind != clang::CastKind::CK_LValueToRValue) {
    visitor_info_.function_info_[current_function_].num_casts++;
  }
  return true;
}

bool CastsVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (!current_function_.empty() && decl->isLocalVarDecl()) {
    visitor_info_.function_info_[current_function_].num_vars++;
  }
  return true;
}