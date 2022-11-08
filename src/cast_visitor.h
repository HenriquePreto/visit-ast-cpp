#ifndef CC_AST_TOOL_CAST_VISITOR_H_
#define CC_AST_TOOL_CAST_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "src/visitor.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "rapidjson/PrettyWriter.h"

class CastVisitor : public Visitor<CastVisitor> {
public:
  struct CastInfo {
    unsigned num_vars_ = 0;
    unsigned begin_line_ = 0;
    unsigned end_line_ = 0;
    std::unordered_multimap<unsigned, clang::CastKind> cast_lines_;
    std::unordered_set<clang::CastKind> cast_kinds_;

    inline unsigned CalculateFunctionSize() const {
      return end_line_ - begin_line_ + 1;
    }
  };

  struct VisitorInfo {
    std::unordered_map<std::string, CastInfo> function_info_;
    
    inline unsigned GetNumCasts(const std::string &function_id) const {
      return function_info_.at(function_id).cast_lines_.size();
    }

    inline unsigned GetNumVars(const std::string &function_id) const {
      return function_info_.at(function_id).num_vars_;
    }

    std::vector<std::pair<const unsigned, clang::CastKind>> GetCastLines(
      const std::string &function_id) const;

    std::vector<clang::CastKind> GetCastKinds(
      const std::string &function_id) const;

    inline unsigned GetFunctionSize(const std::string &function_id) const {
      return function_info_.at(function_id).CalculateFunctionSize();
    }

    inline unsigned GetNumFunctions() const {
      return function_info_.size() - 1; // remove the global context
    }

    void ToJson(
      rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;
  };

  explicit CastVisitor(clang::ASTContext &ctx, VisitorInfo &visitor_info)
    : Visitor(ctx), visitor_info_(visitor_info),
      current_cast_info_(&visitor_info.function_info_[""]) {}

  bool VisitFunctionDecl(const clang::FunctionDecl *decl);

  bool VisitImplicitCastExpr(const clang::ImplicitCastExpr *expr);

  bool VisitVarDecl(const clang::VarDecl *decl);

  bool IsValidImplicitCast(const clang::CastKind &cast_kind) const;

  bool IsLocalStmt(const clang::Stmt *stmt) const;

private:
  VisitorInfo &visitor_info_;
  CastInfo *current_cast_info_;
};

#endif // CC_AST_TOOL_CAST_VISITOR_H_