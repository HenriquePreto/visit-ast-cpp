#ifndef CC_AST_TOOL_GOTO_VISITOR_H_
#define CC_AST_TOOL_GOTO_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <iostream>
#include <string>
#include <unordered_map>

#include "clang/AST/RecursiveASTVisitor.h"
#include "rapidjson/PrettyWriter.h"

class GotoVisitor : public clang::RecursiveASTVisitor<GotoVisitor> {
public:
  struct FunctionInfo {
    unsigned num_gotos_ = 0;
    unsigned num_labels_ = 0;
    std::string body_;
  };

  struct Info {
    std::unordered_map<std::string, FunctionInfo> function_info_;

    inline bool ContainsFunction(const std::string &function_id) const {
      return function_info_.contains(function_id);
    }

    inline unsigned GetNumGotos(const std::string &function_id) const {
      return function_info_.at(function_id).num_gotos_;
    }

    inline unsigned GetNumLabels(const std::string &function_id) const {
      return function_info_.at(function_id).num_labels_;
    }

    inline unsigned GetNumFunctions() const {
      return function_info_.size() - 1; // remove the global context;
    }
    
    void ToJson(
      rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  };

  explicit GotoVisitor(clang::ASTContext &ctx, Info &info)
    : ctx_(ctx), info_(info), 
      current_function_info_(&info.function_info_[""]) {}

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);
  
  bool VisitGotoStmt(const clang::GotoStmt *stmt);

  bool VisitLabelStmt(const clang::LabelStmt *stmt);

private:
  clang::ASTContext &ctx_;
  Info &info_;
  FunctionInfo *current_function_info_;
  clang::Stmt *stmt_;
};

#endif // CC_AST_TOOL_GOTO_VISITOR_H_