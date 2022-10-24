#ifndef CC_AST_TOOL_NOBREAK_VISITOR_H_
#define CC_AST_TOOL_NOBREAK_VISITOR_H_

#ifndef RAPIDJSON_HAS_STDSTRING
#define RAPIDJSON_HAS_STDSTRING 1
#endif

#include "clang/AST/RecursiveASTVisitor.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "rapidjson/PrettyWriter.h"

class NoBreakVisitor : public clang::RecursiveASTVisitor<NoBreakVisitor> {
  public:
    class VisitorInfo {
      public:
        std::unordered_map<std::string, std::string> function_info_;
        
        void ToJson(
          rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
    };

    explicit NoBreakVisitor(clang::ASTContext& ctx, VisitorInfo& visitor_info)
      : ctx_(ctx), visitor_info_(visitor_info), 
        current_function_decl_(nullptr) {}

    bool VisitFunctionDecl(const clang::FunctionDecl* decl);
    
    bool VisitSwitchStmt(const clang::SwitchStmt* stmt);

  private:
    bool hasBreaks(const clang::SwitchStmt* stmt);

    clang::ASTContext& ctx_;
    std::string current_function_;
    VisitorInfo& visitor_info_;
    clang::Stmt* current_function_decl_;
};

#endif // CC_AST_TOOL_NOBREAK_VISITOR_H_