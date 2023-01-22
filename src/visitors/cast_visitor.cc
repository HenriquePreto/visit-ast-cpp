#include "cast_visitor.h"
#include "clang/Basic/SourceManager.h"

void CastVisitor::Info::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const {
  unsigned visited = 0;
  writer.StartObject();
  writer.Key("functions");
  writer.StartArray();
  for (auto const &[key, value] : function_info_) {
    auto num_casts = GetNumCasts(key);
    if (num_casts == 0)
      continue;
    visited++;
    writer.StartObject();
    writer.Key("id");
    writer.String(key);
    writer.Key("casts");
    writer.Uint(num_casts);
    writer.Key("vars");
    writer.Uint(value.num_vars_);
    writer.Key("cast kinds");
    writer.StartArray();
    for (auto const &cast_kind: value.cast_kinds_)
      writer.String(std::string(clang::CastExpr::getCastKindName(cast_kind)));
    writer.EndArray();
    writer.Key("cast lines");
    writer.StartArray();
    for (auto const &[cast_line, cast_kind]: value.cast_lines_) {
      writer.StartObject();
      writer.Key("cast line");
      writer.Uint(cast_line);
      writer.Key("cast kind");
      writer.String(std::string(clang::CastExpr::getCastKindName(cast_kind)));
      writer.EndObject();
    }
    writer.EndArray();
    writer.Key("size");
    writer.Uint(value.CalculateFunctionSize());
    writer.EndObject();
  }
  writer.EndArray();
  writer.Key("total");
  writer.Uint(GetNumFunctions());
  writer.Key("visited");
  writer.Uint(visited);
  writer.EndObject();
}

std::vector<std::pair<const unsigned, clang::CastKind>>
CastVisitor::Info::GetCastLines(const std::string &function_id) const {
  auto &cast_lines = function_info_.at(function_id).cast_lines_;
  return std::vector(cast_lines.cbegin(), cast_lines.cend());
}

std::vector<clang::CastKind> CastVisitor::Info::GetCastKinds(
  const std::string &function_id) const {
    auto &cast_kinds = function_info_.at(function_id).cast_kinds_;
    return std::vector(cast_kinds.cbegin(), cast_kinds.cend());
}

bool CastVisitor::VisitFunctionDecl(const clang::FunctionDecl *decl) {
  if (!(decl->isThisDeclarationADefinition() && decl->hasBody()))
    return true;
  auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
  auto file_name = ctx_.getSourceManager().getFilename(full_location).str();
  auto line_num = full_location.getSpellingLineNumber();
  auto column_num = full_location.getSpellingColumnNumber();
  auto function_name = decl->getQualifiedNameAsString();
  auto function_id = 
    file_name + "#" + std::to_string(line_num) + 
    ":" + std::to_string(column_num)  + "#" + function_name;
  current_function_info_ = &info_.function_info_[function_id];
  auto *body = decl->getBody();
  auto body_begin_loc = ctx_.getFullLoc(body->getBeginLoc());
  auto body_end_loc = ctx_.getFullLoc(body->getEndLoc());
  current_function_info_->begin_line_ = body_begin_loc.getSpellingLineNumber();
  current_function_info_->end_line_ = body_end_loc.getSpellingLineNumber();
  return true;
}

bool CastVisitor::VisitImplicitCastExpr(const clang::ImplicitCastExpr *expr) {
  auto cast_kind = expr->getCastKind();
  if (!(IsLocalStmt(expr) && IsValidImplicitCast(cast_kind)))
    return true;
  auto full_location = ctx_.getFullLoc(expr->getBeginLoc());
  auto line = full_location.getSpellingLineNumber();
  current_function_info_->cast_lines_.emplace(std::make_pair(line, cast_kind));
  current_function_info_->cast_kinds_.emplace(cast_kind);
  return true;
}

bool CastVisitor::VisitVarDecl(const clang::VarDecl *decl) {
  if (decl->isLocalVarDecl())
    ++current_function_info_->num_vars_;
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
  return current_function_info_->begin_line_ <= line_num && 
         line_num <= current_function_info_->end_line_;
}