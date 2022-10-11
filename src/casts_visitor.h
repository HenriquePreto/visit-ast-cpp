#ifndef CC_AST_TOOL_CASTS_VISITOR_H_
#define CC_AST_TOOL_CASTS_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"

class CastsVisitor
  : public clang::RecursiveASTVisitor<CastsVisitor> {
public:
  explicit CastsVisitor(clang::ASTContext *Context)
    : Context(Context) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration);

private:
  clang::ASTContext *Context;
};

#endif // CC_AST_TOOL_CASTS_VISITOR_H_