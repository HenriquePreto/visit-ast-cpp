#include "goto_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

void GotoVisitor::Info::ToJson(
    rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const {
  unsigned visited = 0;
  writer.StartObject();
  writer.Key("functions");
  writer.StartArray();
  for (auto const &[key, value] : function_info_) {
    if (value.num_gotos_ == 0)
      continue;
    visited++;
    writer.StartObject();
    writer.Key("id");
    writer.String(key);
    writer.Key("gotos");
    writer.Uint(value.num_gotos_);
    writer.Key("labels");
    writer.Uint(value.num_labels_);
    writer.Key("body");
    writer.String(value.body_);
    writer.EndObject();
  }
  writer.EndArray();
  writer.Key("total");
  writer.Uint(GetNumFunctions());
  writer.Key("visited");
  writer.Uint(visited);
  writer.EndObject();
}

bool GotoVisitor::VisitFunctionDecl(const clang::FunctionDecl *decl) {
  if (!(decl->isThisDeclarationADefinition() && decl->hasBody())) 
    return true;
  auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
  auto file_name = ctx_.getSourceManager().getFilename(full_location).str();
  auto line_num = full_location.getSpellingLineNumber();
  auto column_num = full_location.getSpellingColumnNumber();
  auto function_name = decl->getQualifiedNameAsString();
  auto function_id = 
    file_name + "#" + std::to_string(line_num) + ":" 
    + std::to_string(column_num)  + "#" + function_name;
  current_function_info_ = &info_.function_info_[function_id];
  stmt_ = decl->getBody();
  return true;
}

bool GotoVisitor::VisitGotoStmt(const clang::GotoStmt *stmt) {
  if (++current_function_info_->num_gotos_ == 1) {
    llvm::raw_string_ostream body_stream(current_function_info_->body_);
    stmt_->printPretty(body_stream, nullptr,
      clang::PrintingPolicy(clang::LangOptions()));
  }
  return true;
}

bool GotoVisitor::VisitLabelStmt(const clang::LabelStmt *stmt) {
  ++current_function_info_->num_labels_;
  return true;
}
