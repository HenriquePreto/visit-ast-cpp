#include "src/casts_visitor.h"

bool CastsVisitor::VisitCastExpr(clang::CastExpr* Expr) {
  clang::FullSourceLoc FullLocation = Context.getFullLoc(Expr->getBeginLoc());
  if (FullLocation.isValid()) {
    std::string castkind = Expr->getCastKindName();
    llvm::outs() << "Found cast expr '"
                  << castkind
                  << "' at "
                  << FullLocation.getSpellingLineNumber() << ":"
                  << FullLocation.getSpellingColumnNumber() << "\n";
  }
  return true;
}