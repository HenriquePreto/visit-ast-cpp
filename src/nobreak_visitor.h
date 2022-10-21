#ifndef CC_AST_TOOL_NOBREAK_VISITOR_H_
#define CC_AST_TOOL_NOBREAK_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include <iostream>
#include <string>
#include <unordered_map>

class NoBreakVisitor : public clang::RecursiveASTVisitor<NoBreakVisitor> {
  public:
    class Collector {
      public:
        std::unordered_map<std::string, std::string> function_info_;
        
        friend std::ostream& operator<<(
          std::ostream& os, const Collector& collector);
    };

    explicit NoBreakVisitor(clang::ASTContext& ctx, Collector& collector)
      : ctx_(ctx), collector_(collector), current_function_decl_(nullptr) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);
    
    bool VisitSwitchStmt(const clang::SwitchStmt* stmt);

  private:
    bool hasBreaks(const clang::SwitchStmt* stmt);

    clang::ASTContext& ctx_;
    std::string current_function_;
    Collector& collector_;
    clang::Stmt* current_function_decl_;
};

#endif // CC_AST_TOOL_NOBREAK_VISITOR_H_