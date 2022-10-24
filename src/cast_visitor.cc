#include "src/cast_visitor.h"
#include "clang/Basic/SourceManager.h"

void CastVisitor::VisitorInfo::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();
  for (auto const& [key, value] : function_info_) {
    writer.String(key);
    writer.StartObject();
    writer.String("casts");
    writer.Uint(value.num_casts_);
    writer.String("vars");
    writer.Uint(value.num_vars_); 
    writer.String("cast kinds");
    writer.StartArray();
    for (auto const& cast_kind: value.cast_kinds_) {
      writer.String(std::string(clang::CastExpr::getCastKindName(cast_kind)));
    }
    writer.EndArray();
    writer.EndObject();
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

bool CastVisitor::VisitImplicitCastExpr(const clang::ImplicitCastExpr* expr) {
  auto cast_kind = expr->getCastKind();
  if (!current_function_.empty() && IsValidImplicitCast(cast_kind)) {
    auto& cast_info = visitor_info_.function_info_[current_function_];
    cast_info.num_casts_++;
    cast_info.cast_kinds_.emplace(cast_kind);
  }
  return true;
}

bool CastVisitor::VisitVarDecl(const clang::VarDecl* decl) {
  if (!current_function_.empty() && decl->isLocalVarDecl()) {
    visitor_info_.function_info_[current_function_].num_vars_++;
  }
  return true;
}

bool CastVisitor::IsValidImplicitCast(const clang::CastKind& cast_kind) const {
  return cast_kind != clang::CastKind::CK_LValueToRValue;
}