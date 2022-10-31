#ifndef CC_AST_TOOL_AST_CONSUMER_H_
#define CC_AST_TOOL_AST_CONSUMER_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"

template <typename T>
class ASTConsumer : public clang::ASTConsumer {
public:
  explicit ASTConsumer(clang::ASTContext &ctx, 
                        typename T::VisitorInfo &visitor_info)
    : visitor_(ctx, visitor_info) {}

  void HandleTranslationUnit(clang::ASTContext &ctx) override;

private:
  T visitor_;
};

template <typename T>
void ASTConsumer<T>::HandleTranslationUnit(clang::ASTContext &ctx) {
  visitor_.TraverseDecl(ctx.getTranslationUnitDecl());
}

#endif // CC_AST_TOOL_AST_CONSUMER_H_