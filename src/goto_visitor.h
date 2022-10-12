#ifndef CC_AST_TOOL_GOTO_VISITOR_H_
#define CC_AST_TOOL_GOTO_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <iostream>
#include <string>
#include <map>

class GotoVisitor : public clang::RecursiveASTVisitor<GotoVisitor> {
  public:
    class Collector {
      public:
        std::map<std::string, std::string> function_info_;
        
        friend std::ostream& operator<<(std::ostream& os, const Collector& collector);
    };

    explicit GotoVisitor(clang::ASTContext& ctx, Collector& collector)
      : ctx_(ctx), collector_(collector), current_function_decl_(nullptr) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);
    
    bool VisitGotoStmt(const clang::GotoStmt* stmt);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    Collector& collector_;
    clang::Stmt* current_function_decl_;
};

#endif // CC_AST_TOOL_GOTO_VISITOR_H_