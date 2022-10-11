#ifndef CC_AST_TOOL_AST_CONSUMER_H_
#define CC_AST_TOOL_AST_CONSUMER_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

template <typename T>
class ASTConsumer : public clang::ASTConsumer {
public:
  explicit ASTConsumer(clang::ASTContext *Context)
    : Visitor(Context) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override;
private:
  T Visitor;
};

template <typename T>
void ASTConsumer<T>::HandleTranslationUnit(clang::ASTContext &Context) {
  Visitor.TraverseDecl(Context.getTranslationUnitDecl());
}

#endif // CC_AST_TOOL_AST_CONSUMER_H_