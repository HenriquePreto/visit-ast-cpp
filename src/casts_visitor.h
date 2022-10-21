#ifndef CC_AST_TOOL_CASTS_VISITOR_H_
#define CC_AST_TOOL_CASTS_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <ostream>
#include <string>
#include <unordered_map>

class CastsVisitor : public clang::RecursiveASTVisitor<CastsVisitor> {
  public:
    class CastsInfo {
      public:
        int num_casts = 0;
        int num_vars = 0;

        friend std::ostream& operator<<(
          std::ostream& os, const CastsInfo& casts_info);
    };

    class Collector {
      public:
        std::unordered_map<std::string, CastsInfo> function_info_;
        
        friend std::ostream& operator<<(
          std::ostream& os, const Collector& collector);
    };

    explicit CastsVisitor(clang::ASTContext& ctx, Collector& collector)
      : ctx_(ctx), collector_(collector) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);

    bool VisitCastExpr(const clang::CastExpr* expr);

    bool VisitVarDecl(const clang::VarDecl* decl);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    Collector& collector_;
};

#endif // CC_AST_TOOL_CASTS_VISITOR_H_