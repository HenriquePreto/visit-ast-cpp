#include "src/cast_visitor.h"
#include "clang/Basic/SourceManager.h"

void CastVisitor::VisitorInfo::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const {
  writer.StartArray();
  for (auto const &[key, value] : function_info_) {
    if (value.num_casts_ == 0)
      continue;
    writer.StartObject();
    writer.Key("filename");
    writer.String(key);
    writer.Key("casts");
    writer.Uint(value.num_casts_);
    writer.Key("vars");
    writer.Uint(value.num_vars_);
    writer.Key("cast kinds");
    writer.StartArray();
    for (auto const &cast_kind: value.cast_kinds_) {
      writer.String(std::string(clang::CastExpr::getCastKindName(cast_kind)));
    }
    writer.EndArray();
    writer.Key("function size");
    writer.Uint(value.CalculateFunctionSize());
    writer.EndObject();
  }
  writer.EndArray();
}

bool CastVisitor::VisitFunctionDecl(const clang::FunctionDecl *decl) {
  if (decl->isThisDeclarationADefinition()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto file_name = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto column_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    auto function_id = 
      file_name + "#" + std::to_string(line_num) + 
      ":" + std::to_string(column_num)  + "#" + function_name;
    current_function_ = &visitor_info_.function_info_[function_id];
    auto *body = decl->getBody();
    auto body_begin_loc = ctx_.getFullLoc(body->getBeginLoc());
    auto body_end_loc = ctx_.getFullLoc(body->getEndLoc());
    current_function_->begin_line_ = body_begin_loc.getSpellingLineNumber();
    current_function_->end_line_ = body_end_loc.getSpellingLineNumber();
  }
  return true;
}

bool CastVisitor::VisitImplicitCastExpr(const clang::ImplicitCastExpr *expr) {
  auto cast_kind = expr->getCastKind();
  if (IsLocalStmt(expr) && IsValidImplicitCast(cast_kind)) {
    ++current_function_->num_casts_;
    current_function_->cast_kinds_.emplace(cast_kind);
  }
  return true;
}

bool CastVisitor::VisitVarDecl(const clang::VarDecl *decl) {
  if (decl->isLocalVarDecl()) {
    ++current_function_->num_vars_;
  }
  return true;
}

bool CastVisitor::IsValidImplicitCast(const clang::CastKind &cast_kind) const {
  return cast_kind != clang::CastKind::CK_LValueToRValue &&
         cast_kind != clang::CastKind::CK_FunctionToPointerDecay &&
         cast_kind != clang::CastKind::CK_NoOp &&
         cast_kind != clang::CastKind::CK_ArrayToPointerDecay;
}

bool CastVisitor::IsLocalStmt(const clang::Stmt *stmt) const {
  auto full_location = ctx_.getFullLoc(stmt->getBeginLoc());
  auto line_num = full_location.getSpellingLineNumber();
  return current_function_->begin_line_ <= line_num && 
         line_num <= current_function_->end_line_;
}