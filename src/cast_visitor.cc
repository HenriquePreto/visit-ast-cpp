#include "src/cast_visitor.h"
#include "clang/Basic/SourceManager.h"

void CastVisitor::CastInfo::to_json(
    rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();
  writer.String("casts");
  writer.Uint(num_casts_);
  writer.String("vars");
  writer.Uint(num_vars_); 
  writer.EndObject();
}

void CastVisitor::VisitorInfo::to_json(
    rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();
  for (auto const& [key, value] : function_info_) {
    writer.String(key);
    value.to_json(writer);
  }
  writer.EndObject();
}

bool CastVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
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

bool CastVisitor::VisitCastExpr(const clang::CastExpr* expr) {
  auto cast_kind = expr->getCastKind();
  if (!current_function_.empty() && 
      cast_kind != clang::CastKind::CK_LValueToRValue) {
    visitor_info_.function_info_[current_function_].num_casts_++;
  }
  return true;
}

bool CastVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (!current_function_.empty() && decl->isLocalVarDecl()) {
    visitor_info_.function_info_[current_function_].num_vars_++;
  }
  return true;
}