#ifndef CC_AST_TOOL_AST_CONSUMER_H_
#define CC_AST_TOOL_AST_CONSUMER_H_

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/Basic/SourceManager.h"

template <typename T>
class ASTConsumer : public clang::ASTConsumer {
public:
  explicit ASTConsumer(clang::ASTContext &ctx, 
                        typename T::VisitorInfo &visitor_info)
    : visitor_(ctx, visitor_info) {}

  void HandleTranslationUnit(clang::ASTContext &ctx) override;

  bool IsTraversable(
    clang::ASTContext &ctx, const clang::SourceLocation &begin_loc) const;

private:
  T visitor_;
};

template <typename T>
void ASTConsumer<T>::HandleTranslationUnit(clang::ASTContext &ctx) {
  auto *decl = ctx.getTranslationUnitDecl();

  auto begin = decl->decls_begin();
  auto end = decl->decls_end();
  auto it = std::find_if(begin, end, [this, &ctx] (auto *child_decl) {
    return IsTraversable(ctx, child_decl->getBeginLoc());
  });

  for (auto continue_traversal = true; it != end && continue_traversal; ++it) {
    continue_traversal = visitor_.TraverseDecl(*it);
  }
}

template <typename T>
bool ASTConsumer<T>::IsTraversable(clang::ASTContext &ctx,
    const clang::SourceLocation &begin_loc) const {
  auto &src_mgr = ctx.getSourceManager();
  return src_mgr.isInMainFile(begin_loc);
}

#endif // CC_AST_TOOL_AST_CONSUMER_H_