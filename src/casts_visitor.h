#ifndef CC_AST_TOOL_CASTS_VISITOR_H_
#define CC_AST_TOOL_CASTS_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <string>
#include <map>

class CastsVisitor : public clang::RecursiveASTVisitor<CastsVisitor> {
  class CastsInfo {
    public:
      int num_casts = 0;
      int num_vars = 0;
  };

  public:
    explicit CastsVisitor(clang::ASTContext& ctx)
      : ctx_(ctx) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);

    bool VisitCastExpr(const clang::CastExpr* expr);

    bool VisitVarDecl(const clang::VarDecl* decl);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    std::map<std::string, CastsInfo> function_info_;
};

#endif // CC_AST_TOOL_CASTS_VISITOR_H_