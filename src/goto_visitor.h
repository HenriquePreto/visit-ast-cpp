#ifndef CC_AST_TOOL_GOTO_VISITOR_H_
#define CC_AST_TOOL_GOTO_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "src/visitor.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "rapidjson/PrettyWriter.h"

class GotoVisitor : public Visitor<GotoVisitor> {
public:
  struct GotoInfo {
    unsigned num_gotos_ = 0;
    std::string body_;
  };

  struct VisitorInfo {
    std::unordered_map<std::string, GotoInfo> function_info_;

    inline bool ContainsFunction(const std::string &function_id) const {
      return function_info_.contains(function_id);
    }

    inline unsigned GetNumGotos(const std::string &function_id) const {
      return function_info_.at(function_id).num_gotos_;
    }

    inline unsigned GetNumFunctions() const {
      return function_info_.size() - 1; // remove the global context;
    }
    
    void ToJson(
      rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  };

  explicit GotoVisitor(clang::ASTContext &ctx, VisitorInfo &visitor_info)
    : Visitor(ctx), visitor_info_(visitor_info), 
      current_goto_info_(&visitor_info.function_info_[""]) {}

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);
  
  bool VisitGotoStmt(const clang::GotoStmt *stmt);

private:
  VisitorInfo &visitor_info_;
  GotoInfo *current_goto_info_;
  clang::Stmt *stmt_;
};

#endif // CC_AST_TOOL_GOTO_VISITOR_H_