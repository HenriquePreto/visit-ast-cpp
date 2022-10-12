#include "src/casts_visitor.h"
#include "clang/Basic/SourceManager.h"

std::ostream& operator<<(std::ostream& os, const CastsVisitor::CastsInfo& casts_info) {
  os << "(num_casts=" 
    << casts_info.num_casts 
    << ", num_vars=" 
    << casts_info.num_vars 
    << ", freq="
    << static_cast<double>(casts_info.num_casts) / casts_info.num_vars
    << ")";
  return os;
}

std::ostream& operator<<(std::ostream& os, const CastsVisitor::Collector& collector) {
  os << "CastVisitorInfo {" << std::endl;
  for (auto const& [key, val] : collector.function_info_) {
    os << "\t"
        << key
        << " -> "
        << val
        << ", "
        << std::endl;
  }
  os << "}";  
  return os;
}

bool CastsVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto col_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    current_function_ = 
      fname + "#" + std::to_string(line_num) + ":" + std::to_string(col_num)  + "#" + function_name;
  }
  return true;
}

bool CastsVisitor::VisitCastExpr(const clang::CastExpr* expr) {
  std::string cast_kind = expr->getCastKindName();
  if (current_function_ != "" && cast_kind != "LValueToRValue") {
    if (collector_.function_info_.find(current_function_) == collector_.function_info_.end()) {
      collector_.function_info_[current_function_] = CastsInfo();
    }
    collector_.function_info_[current_function_].num_casts++;
  }
  return true;
}

bool CastsVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (current_function_ != "" && decl->isLocalVarDecl()) {
    if (collector_.function_info_.find(current_function_) == collector_.function_info_.end()) {
      collector_.function_info_[current_function_] = CastsInfo();
    }
    collector_.function_info_[current_function_].num_vars++;
  }
  return true;
}