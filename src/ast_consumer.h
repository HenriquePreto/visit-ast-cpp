#ifndef CC_AST_TOOL_AST_CONSUMER_H_
#define CC_AST_TOOL_AST_CONSUMER_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "src/casts_visitor.h"

class ASTConsumer : public clang::ASTConsumer {
public:
  explicit ASTConsumer(clang::ASTContext *Context)
    : Visitor(Context) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override;
private:
  CastsVisitor Visitor;
};

#endif // CC_AST_TOOL_AST_CONSUMER_H_