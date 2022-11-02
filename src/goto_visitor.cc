#include "goto_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

void GotoVisitor::VisitorInfo::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const {
  writer.StartArray();
  for (auto const &[key, value] : function_info_) {
    if (value.num_gotos_ == 0)
      continue;
    writer.StartObject();
    writer.Key("id");
    writer.String(key);
    writer.Key("gotos");
    writer.Uint(value.num_gotos_);
    writer.Key("body");
    writer.String(value.body_);
    writer.EndObject();
  }
  writer.EndArray();
}

bool GotoVisitor::VisitFunctionDecl(const clang::FunctionDecl *decl) {
  // Expr `decl->hasBody()` is needed because a default definition 
  // doesn't have body: `virtual ~SomeClass() = default`;
  if (decl->isThisDeclarationADefinition() && decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto file_name = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto column_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    auto function_id = 
      file_name + "#" + std::to_string(line_num) + ":" 
      + std::to_string(column_num)  + "#" + function_name;
    current_goto_info_ = &visitor_info_.function_info_[function_id];
    stmt_ = decl->getBody();
  }
  return true;
}

bool GotoVisitor::VisitGotoStmt(const clang::GotoStmt *stmt) {
  if (++current_goto_info_->num_gotos_ == 1) {
    llvm::raw_string_ostream body_stream(current_goto_info_->body_);
    stmt_->printPretty(body_stream, nullptr,
      clang::PrintingPolicy(clang::LangOptions()));
  }
  return true;
}