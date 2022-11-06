#ifndef CC_AST_TOOL_VISITOR_H_
#define CC_AST_TOOL_VISITOR_H_

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTContext.h"

template <typename T>
class Visitor : public clang::RecursiveASTVisitor<T> {
public:
  explicit Visitor(clang::ASTContext &ctx)
    : ctx_(ctx) {}
  
protected:
  clang::ASTContext &ctx_;
};

#endif // CC_AST_TOOL_VISITOR_H_