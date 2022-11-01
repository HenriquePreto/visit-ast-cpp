#ifndef CC_AST_TOOL_NOBREAK_VISITOR_H_
#define CC_AST_TOOL_NOBREAK_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "rapidjson/PrettyWriter.h"

class NoBreakVisitor : public clang::RecursiveASTVisitor<NoBreakVisitor> {
public:
  struct NoBreakInfo {
    unsigned num_nobreaks_ = 0;
    clang::Stmt *stmt_;
  };

  struct VisitorInfo {
    std::unordered_map<std::string, NoBreakInfo> function_info_;
    
    inline bool ContainsFunction(const std::string &function_id) const {
      return function_info_.contains(function_id);
    }

    inline unsigned GetNumNoBreaks(const std::string &function_id) const {
      return function_info_.at(function_id).num_nobreaks_;
    }

    inline int GetNumFunctions() const {
      return function_info_.size() - 1; // remove the global context;
    }

    void ToJson(
      rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  };

  explicit NoBreakVisitor(clang::ASTContext &ctx, VisitorInfo &visitor_info)
    : ctx_(ctx), visitor_info_(visitor_info), 
      current_nobreak_info_(&visitor_info.function_info_[""]) {}

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);
  
  bool VisitSwitchStmt(const clang::SwitchStmt *stmt);

private:
  bool IsOkSwitch(const clang::SwitchStmt *stmt) const;

  bool HasBreakChild(const clang::ConstStmtIterator &it) const;

  bool IsBreakBelow(
    clang::ConstStmtIterator &it, const clang::ConstStmtIterator &end) const;

  bool IsFallThroughCase(clang::ConstStmtIterator it) const;

  bool AssignIfHasCaseChild(clang::ConstStmtIterator &it) const;

  bool IsCompoundStmt(const clang::ConstStmtIterator &it) const;

  clang::ASTContext &ctx_;
  VisitorInfo &visitor_info_;
  NoBreakInfo *current_nobreak_info_;
};

#endif // CC_AST_TOOL_NOBREAK_VISITOR_H_