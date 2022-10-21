#ifndef CC_AST_TOOL_CAST_VISITOR_H_
#define CC_AST_TOOL_CAST_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include <string>
#include <unordered_map>

#include "clang/AST/RecursiveASTVisitor.h"
#include "rapidjson/PrettyWriter.h"

class CastVisitor : public clang::RecursiveASTVisitor<CastVisitor> {
  public:
    class CastInfo {
      public:
        int num_casts_ = 0;
        int num_vars_ = 0;

        void to_json(
          rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
    };

    class VisitorInfo {
      public:
        std::unordered_map<std::string, CastInfo> function_info_;
        
        void to_json(
          rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
    };

    explicit CastVisitor(clang::ASTContext& ctx, VisitorInfo& visitor_info)
      : ctx_(ctx), visitor_info_(visitor_info) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);

    bool VisitCastExpr(const clang::CastExpr* expr);

    bool VisitVarDecl(const clang::VarDecl* decl);

  private:
    clang::ASTContext& ctx_;
    std::string current_function_;
    VisitorInfo& visitor_info_;
};

#endif // CC_AST_TOOL_CAST_VISITOR_H_