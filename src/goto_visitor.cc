#include "goto_visitor.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/Support/raw_ostream.h"

void GotoVisitor::VisitorInfo::to_json(
    rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const {
  writer.StartObject();
  for (auto const& [key, value] : function_info_) {
    writer.String(key);
    writer.String(value);
  }
  writer.EndObject();
}

bool GotoVisitor::VisitFunctionDecl(const clang::FunctionDecl* decl) {
  if (decl->hasBody()) {
    auto full_location = ctx_.getFullLoc(decl->getBeginLoc());
    auto fname = ctx_.getSourceManager().getFilename(full_location).str();
    auto line_num = full_location.getSpellingLineNumber();
    auto column_num = full_location.getSpellingColumnNumber();
    auto function_name = decl->getQualifiedNameAsString();
    current_function_ = 
      fname + "#" + std::to_string(line_num) + ":" 
      + std::to_string(column_num)  + "#" + function_name;
    current_function_decl_ = decl->getBody();
  }
  return true;
}

bool GotoVisitor::VisitGotoStmt(const clang::GotoStmt* stmt) {
  if (!current_function_.empty() && current_function_decl_ != nullptr) {
    std::string body;
    llvm::raw_string_ostream body_stream(body);
    current_function_decl_->printPretty(body_stream, 
      nullptr, clang::PrintingPolicy(clang::LangOptions()));
    visitor_info_.function_info_[current_function_] = body;
  }
  return true;
}