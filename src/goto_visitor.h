#ifndef CC_AST_TOOL_GOTO_VISITOR_H_
#define CC_AST_TOOL_GOTO_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <iostream>
#include <string>
#include <unordered_map>

class GotoVisitor : public clang::RecursiveASTVisitor<GotoVisitor> {
  public:
    class VisitorInfo {
      public:
        std::unordered_map<std::string, std::string> function_info_;
        
        friend std::ostream& operator<<(
          std::ostream& os, const VisitorInfo& visitor_info);
    };

    explicit GotoVisitor(clang::ASTContext& ctx, VisitorInfo& visitor_info)
      : ctx_(ctx), visitor_info_(visitor_info), 
        current_function_decl_(nullptr) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);
    
    bool VisitGotoStmt(const clang::GotoStmt* stmt);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    VisitorInfo& visitor_info_;
    clang::Stmt* current_function_decl_;
};

#endif // CC_AST_TOOL_GOTO_VISITOR_H_