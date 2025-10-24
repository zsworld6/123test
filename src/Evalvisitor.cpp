#include "Evalvisitor.h"
#include "Scope.h"
#include "Func.h"
#include "operator.h"
#include <any>
#include <cassert>
#include <cstddef>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include "InnerFunc.h"

Scope scope;
Function func;

void varBack(std::any &p) {
  if (p.type() != typeid(std::pair<std::string, bool>)) {
    return ;
  }
  auto varname = std::any_cast<std::pair<std::string, bool>>(p).first;
  if (scope.VarFind(varname)) {
    p = scope.VarQuery(varname);
  }
} 

bool isFlow(const std::any &a) {
  return a.type() == typeid(FLOWCONDITION) && std::any_cast<FLOWCONDITION>(a) != kNOTFLOW;
}

std::any EvalVisitor::visitAtom(Python3Parser::AtomContext *ctx) {
  if (ctx->NAME()) {
    auto p = ctx->NAME()->getText();
    return std::make_pair(p, true);
  } else if (ctx->NUMBER()) {
    auto s = ctx->NUMBER()->getText();
    if (s.find('.') != s.npos) {
      return GetFlout(s);
    } else {
      return GetInt(s);
    }
  } else if (ctx->NONE()) {
    return std::any();
  } else if (ctx->TRUE()) {
    return true;
  } else if (ctx->FALSE()) {
    return false;
  } else if (!ctx->STRING().empty()) {
    auto p = ctx->STRING();
    std::string res;
    for (const auto &x : p) {
      auto s = x->getText();
      s.erase(0, 1);
      s.pop_back();
      res += s;
    }
    return res;
  } else if (ctx->test()) {
    return visit(ctx->test());
  } else if(ctx->format_string()) {
    return visit(ctx->format_string());
  } else {
    return nullptr;
  }
}

std::any EvalVisitor::visitFactor(Python3Parser::FactorContext *ctx) {
  std::any res;
  if (ctx->factor()) {
    res = visitFactor(ctx->factor());
  } else {
    res = visitAtom_expr(ctx->atom_expr());
  }
  if (ctx->MINUS()) {
    varBack(res);
    return -res;
  } else {
    return res;
  }
}

std::any EvalVisitor::visitTerm(Python3Parser::TermContext *ctx) {
  auto factorarray = ctx->factor();
  auto res = visit(factorarray[0]);
  if (factorarray.size() == 1u) {
    return res;
  }
  varBack(res);
  auto muldivmod_oparray = ctx->muldivmod_op();
  for (size_t i = 1; i < factorarray.size(); i++) {
    auto x = visit(factorarray[i]);
    varBack(x);
    auto op = std::any_cast<std::string>(visit(muldivmod_oparray[i - 1]));
    if (op == "*") {
      res *= x;
    } else if (op == "/") {
      res /= x;
    } else if (op == "%") {
      res %= x;
    } else if (op == "//") {
      ForceDivEqual(res, x);
    }
  }
  return res;
}

std::any EvalVisitor::visitArith_expr(Python3Parser::Arith_exprContext *ctx) {
  auto termarray = ctx->term();
  auto res = visit(termarray[0]);
  if (termarray.size() == 1u) {
    return res;
  }
  varBack(res);
  auto addorsub_oparray = ctx->addorsub_op();
  for (size_t i = 1; i < termarray.size(); i++) {
    auto x = visit(termarray[i]);
    varBack(x);
    auto p = visit(addorsub_oparray[i - 1]);
    auto op = std::any_cast<std::string>(visit(addorsub_oparray[i - 1]));
    if (op == "+") {
      res += x;
    } else if (op == "-") {
      res -= x;
    }
  }
  return res;
}

std::any EvalVisitor::visitFile_input(Python3Parser::File_inputContext *ctx) {
  return visitChildren(ctx);
}

std::any EvalVisitor::visitFuncdef(Python3Parser::FuncdefContext *ctx) {
  auto name = ctx->NAME()->getText();
  auto val = visit(ctx->parameters());
  auto suite = ctx->suite();
  func.AddFunc(name, std::any_cast<std::vector<std::pair<std::string, std::any>>>(val), suite);
  return kNOTFLOW;
}

std::any EvalVisitor::visitParameters(Python3Parser::ParametersContext *ctx) {
  if (!ctx->typedargslist()) {
    return std::vector<std::pair<std::string, std::any>>();
  } else {
    return visit(ctx->typedargslist());
  }
}

std::any EvalVisitor::visitTypedargslist(Python3Parser::TypedargslistContext *ctx) {
  std::vector<std::string> namelist;
  std::vector<std::any> vallist;
  auto tfpdefarray = ctx->tfpdef();
  auto testarray = ctx->test();
  for (const auto &x : tfpdefarray) {
    namelist.push_back(std::any_cast<std::string>(visit(x)));
  }
  for (const auto &x : testarray) {
    auto p = visit(x);
    varBack(p);
    vallist.push_back(p);
  }
  std::vector<std::pair<std::string, std::any>> res;
  for (size_t i = 0; i + vallist.size() < namelist.size(); i++) {
    res.push_back(std::make_pair(namelist[i], std::any()));
  }
  for (size_t i = 0; i < vallist.size(); i++) {
    res.push_back(std::make_pair(namelist[i + namelist.size() - vallist.size()], vallist[i]));
  }
  return res;
}

std::any EvalVisitor::visitTfpdef(Python3Parser::TfpdefContext *ctx) {
  return ctx->NAME()->getText();
}

std::any EvalVisitor::visitStmt(Python3Parser::StmtContext *ctx) {
  if (ctx->simple_stmt()) {
    return visit(ctx->simple_stmt());
  } else {
    return visit(ctx->compound_stmt());
  }
}

std::any EvalVisitor::visitSimple_stmt(Python3Parser::Simple_stmtContext *ctx) {
  return visit(ctx->small_stmt());
}

std::any EvalVisitor::visitSmall_stmt(Python3Parser::Small_stmtContext *ctx) {
  if (ctx->expr_stmt()) {
    return visit(ctx->expr_stmt());
  } else if (ctx->flow_stmt()) {
    return visit(ctx->flow_stmt());
  } else {
    assert("INVALID SMALLSTMT");
    return nullptr;
  }
}

std::any EvalVisitor::visitExpr_stmt(Python3Parser::Expr_stmtContext *ctx) {
  auto testlistarray = ctx->testlist();
  if (testlistarray.size() == 1u) {
    visit(testlistarray[0]);
    return kNOTFLOW;
  }
  if (!ctx->augassign()) {
    std::vector<std::any> leftarray, rightarray, tmp;
    tmp = std::any_cast<std::vector<std::any>>(visit(*(testlistarray.rbegin())));
    for (const auto &p : tmp) {
      if (p.type() == typeid(std::vector<std::any>)) {
        auto array = std::any_cast<std::vector<std::any>>(p);
        for (const auto &q : array) {
          rightarray.push_back(q);
        }
      } else {
        rightarray.push_back(p);
      }
    }
    for (int i = static_cast<int>(testlistarray.size()) - 2; i >= 0; i--) {
      leftarray = std::any_cast<std::vector<std::any>>(visit(testlistarray[i]));
      for (size_t j = 0; j < rightarray.size(); j++) {
        auto p = rightarray[j];
        varBack(p);
        scope.VarSet(std::any_cast<std::pair<std::string, bool>>(leftarray[j]).first, p);
      }
      rightarray = leftarray;
    }
  } else {
    auto x = visit(testlistarray[0]);
    auto op = std::any_cast<std::string>(visit(ctx->augassign()));
    auto y = visit(testlistarray[1]);
    y = std::any_cast<std::vector<std::any>>(y)[0];
    varBack(y);
    auto name = std::any_cast<std::pair<std::string, bool>>(std::any_cast<std::vector<std::any>>(x)[0]).first;
    auto val = scope.VarQuery(name);
    if (op == "+=") {
      val += y;
    } else if (op == "-=") {
      val -= y;
    } else if (op == "*=") {
      val *= y;
    } else if (op == "/=") {
      val /= y;
    } else if (op == "//=") {
      ForceDivEqual(val, y);
    } else if (op == "%=") {
      val %= y;
    }
    scope.VarSet(name, val);
  }
  return kNOTFLOW;
}

std::any EvalVisitor::visitAugassign(Python3Parser::AugassignContext *ctx) {
  if (ctx->ADD_ASSIGN()) {
    return std::string("+=");
  } else if (ctx->SUB_ASSIGN()) {
    return std::string("-=");
  } else if (ctx->MULT_ASSIGN()) {
    return std::string("*=");
  } else if (ctx->DIV_ASSIGN()) {
    return std::string("/=");
  } else if (ctx->IDIV_ASSIGN()) {
    return std::string("//=");
  } else if (ctx->MOD_ASSIGN()) {
    return std::string("%=");
  } else {
    assert("INVALID AUGASSIGN" == 0);
  }
}

std::any EvalVisitor::visitFlow_stmt(Python3Parser::Flow_stmtContext *ctx) {
  if (ctx->break_stmt()) {
    return visit(ctx->break_stmt());
  } else if (ctx->continue_stmt()) {
    return visit(ctx->continue_stmt());
  } else if (ctx->return_stmt()) {
    return visit(ctx->return_stmt());
  } else {
    assert("INVALID FLOW" == 0);
  }
}

std::any EvalVisitor::visitBreak_stmt(Python3Parser::Break_stmtContext *ctx) {
  return kBREAK;
}

std::any EvalVisitor::visitContinue_stmt(Python3Parser::Continue_stmtContext *ctx) {
  return kCONTINUE;
}

std::any EvalVisitor::visitReturn_stmt(Python3Parser::Return_stmtContext *ctx) {
  if (!ctx->testlist()) {
    return std::any();
  } else {
    auto valarray = std::any_cast<std::vector<std::any>>(visit(ctx->testlist()));
    for (auto &p : valarray) {
      varBack(p);
    }
    if (valarray.size() == 1) {
      return valarray[0];
    } else {
      return valarray;
    }
  }
}

std::any EvalVisitor::visitCompound_stmt(Python3Parser::Compound_stmtContext *ctx) {
  if (ctx->if_stmt()) {
    return visit(ctx->if_stmt());
  } else if (ctx->while_stmt()) {
    return visit(ctx->while_stmt());
  } else if (ctx->funcdef()) {
    return visit(ctx->funcdef());
  } else {
    assert("INVALID COMPOUNDSTMT" == 0);
  }
}

std::any EvalVisitor::visitIf_stmt(Python3Parser::If_stmtContext *ctx) {
  auto testarray = ctx->test();
  auto suitearray = ctx->suite();
  for (size_t i = 0; i < testarray.size(); i++) {
    auto p = visit(testarray[i]);
    varBack(p);
    if (GetBool(p)) {
      return visit(suitearray[i]);
    }
  }
  if (suitearray.size() == testarray.size() + 1) {
    return visit(suitearray.back());
  }
  return kNOTFLOW;
}

std::any EvalVisitor::visitWhile_stmt(Python3Parser::While_stmtContext *ctx) {
  while (1) {
    auto check = visit(ctx->test());
    varBack(check);
    if (!GetBool(check)) {
      break;
    }
    auto p = visit(ctx->suite());
    if (p.type() != typeid(FLOWCONDITION)) {
      return p;
    } else {
      if (std::any_cast<FLOWCONDITION>(p) == kBREAK) {
        break;
      }
    }
  }
  return kNOTFLOW;
}

std::any EvalVisitor::visitSuite(Python3Parser::SuiteContext *ctx) {
  if (ctx->simple_stmt()) {
    return visit(ctx->simple_stmt());
  }
  auto stmtarray = ctx->stmt();
  for (size_t i = 0; i < stmtarray.size(); i++) {
    auto p = visit(stmtarray[i]);
    if (p.type() != typeid(FLOWCONDITION)) {
      return p;
    }
    if (isFlow(p)) {
      return p;
    }
  }
  return kNOTFLOW;
}

std::any EvalVisitor::visitTest(Python3Parser::TestContext *ctx) {
  auto p = visit(ctx->or_test());
  return p;
}

std::any EvalVisitor::visitOr_test(Python3Parser::Or_testContext *ctx) {
  auto andtestarray = ctx->and_test();
  auto res = visit(andtestarray[0]);
  if (andtestarray.size() == 1u) {
    return res;
  }
  varBack(res);
  if (GetBool(res)) {
    return true;
  }
  for (size_t i = 1; i < andtestarray.size(); i++) {
    auto p = visit(andtestarray[i]);
    varBack(p);
    if (GetBool(p)) {
      return true;
    }
  }
  return false;
}

std::any EvalVisitor::visitAnd_test(Python3Parser::And_testContext *ctx) {
  auto nottestarray = ctx->not_test();
  auto res = visit(nottestarray[0]);
  if (nottestarray.size() == 1u) {
    return res;
  }
  varBack(res);
  if (!GetBool(res)) {
    return false;
  }
  for (size_t i = 1; i < nottestarray.size(); i++) {
    auto p = visit(nottestarray[i]);
    varBack(p);
    if (!GetBool(p)) {
      return false;
    }
  }
  return true;
}

std::any EvalVisitor::visitNot_test(Python3Parser::Not_testContext *ctx) {
  if (ctx->NOT()) {
    auto p = visit(ctx->not_test());
    varBack(p);
    return !GetBool(p);
  } else {
    return visit(ctx->comparison());
  }
}

std::any EvalVisitor::visitComparison(Python3Parser::ComparisonContext *ctx) {
  auto arritharray = ctx->arith_expr();
  auto res = visit(arritharray[0]);
  if (arritharray.size() == 1u) {
    return res;
  }
  varBack(res);
  auto compoparray = ctx->comp_op();
  for (size_t i = 1; i < arritharray.size(); i++) {
    auto p = visit(arritharray[i]);
    varBack(p);
    auto op = std::any_cast<std::string>(visit(compoparray[i - 1]));
    if (op == "<") {
      if (!(res < p)) {
        return false;
      }
    } else if (op == ">") {
      if (!(res > p)) {
        return false;
      }
    } else if (op == "==") {
      if (!(res == p)) {
        return false;
      }
    } else if (op == "<=") {
      if (!(res <= p)) {
        return false;
      }
    } else if (op == ">=") {
      if (!(res >= p)) {
        return false;
      }
    } else if (op == "!=") {
      if (!(res != p)) {
        return false;
      }
    }
    res = p;
  }
  return true;
}

std::any EvalVisitor::visitComp_op(Python3Parser::Comp_opContext *ctx) {
  if (ctx->EQUALS()) {
    return std::string("==");
  } else if (ctx->GREATER_THAN()) {
    return std::string(">");
  } else if (ctx->LESS_THAN()) {
    return std::string("<");
  } else if (ctx->GT_EQ()) {
    return std::string(">=");
  } else if (ctx->LT_EQ()) {
    return std::string("<=");
  } else if (ctx->NOT_EQ_2()) {
    return std::string("!=");
  } else {
    // throw INVALID COMPARISON
    assert("INVALID COMPARISON" == 0);
  }
}

std::any EvalVisitor::visitAddorsub_op(Python3Parser::Addorsub_opContext *ctx) {
  if (ctx->ADD()) {
    return std::string("+");
  } else if (ctx->MINUS()) {
    return std::string("-");
  } else {
    assert("???" == 0);
  }
}

std::any EvalVisitor::visitMuldivmod_op(Python3Parser::Muldivmod_opContext *ctx) {
  if (ctx->STAR()) {
    return std::string("*");
  } else if (ctx->DIV()) {
    return std::string("/");
  } else if (ctx->IDIV()) {
    return std::string("//");
  } else if (ctx->MOD()) {
    return std::string("%");
  } else {
    // throw unvalid muldivmod_op
    assert("unvalid muldivmod_op" == 0);
  }
}

std::any EvalVisitor::visitAtom_expr(Python3Parser::Atom_exprContext *ctx) {
  if (ctx->trailer()) {
    auto funcname = std::any_cast<std::pair<std::string, bool>>(visit(ctx->atom())).first;
    auto vallist = std::any_cast<std::pair<std::vector<std::any>, std::vector<std::pair<std::string, std::any>>>>(visit(ctx->trailer()));
    auto tmp = vallist.first;
    auto keyboard = vallist.second;
    std::vector<std::any> position;
    for (const auto &p : tmp) {
      if (p.type() == typeid(std::vector<std::any>)) {
        auto array = std::any_cast<std::vector<std::any>>(p);
        for (const auto &q : array) {
          position.push_back(q);
        }
      } else {
        position.push_back(p);
      }
    }
    if (CheckInner(funcname)) {
      std::vector<std::any> val;
      for (const auto &x : position) {
        val.push_back(x);
      }
      for (const auto &x : keyboard) {
        val.push_back(x.second);
      }
      return Inner(funcname, val);
    }
    scope.LayerCreate();
    auto suite = func.GetSuite(funcname);
    auto initval = func.GetInitial(funcname);
    std::unordered_set<std::string> used;
    for (size_t i = 0; i < position.size(); i++) {
      scope.VarRegister(initval[i].first, position[i]);
    }
    for (size_t i = 0; i < keyboard.size(); i++) {
      scope.VarRegister(keyboard[i].first, keyboard[i].second);
      used.insert(keyboard[i].first);
    }
    for (size_t i = position.size(); i < initval.size(); i++) {
      if (used.find(initval[i].first) == used.end()) {
        scope.VarRegister(initval[i].first, initval[i].second);
      }
    }
    auto res = visit(suite);
    scope.LayerDelete();
    return res;
  } else {
    return visit(ctx->atom());
  }
}

std::any EvalVisitor::visitTrailer(Python3Parser::TrailerContext *ctx) {
  if (!ctx->arglist()) {
    return std::pair<std::vector<std::any>, std::vector<std::pair<std::string, std::any>>>();
  } else {
    return visit(ctx->arglist());
  }
}

std::any EvalVisitor::visitFormat_string(Python3Parser::Format_stringContext *ctx) {
  auto stringarray = ctx->FORMAT_STRING_LITERAL();
  auto testlistarray = ctx->testlist();
  auto s = ctx->getText();
  s.pop_back();
  std::string res;
  size_t pos0 = 0, pos1 = 0;
  for (size_t i = 2; i < s.size(); i++) {
    if (s[i] != '{' || (i + 1 < s.size() && s[i] == '{' && s[i + 1] == '{')) {
      auto tmp =  stringarray[pos0++]->getText();
      for (size_t j = 0; j < tmp.size(); j++) {
        if (tmp[j] == '{') {
          res += '{';
          j++;
        } else if (tmp[j] == '}') {
          res += '}';
          j++;
        } else {
          res += tmp[j];
        }
      }
      i += tmp.size() - 1;
    } else {
      auto p = std::any_cast<std::vector<std::any>>(visit(testlistarray[pos1++]))[0];
      varBack(p);
      res += GetString(p);
      int cnt = 0;
      while (i < s.size()) {
        if (s[i] == '{') cnt++;
        else if (s[i] == '}') cnt--;
        if (!cnt) {
          break;
        }
        i++;
      }
    }
  }
  return res;
}

std::any EvalVisitor::visitTestlist(Python3Parser::TestlistContext *ctx) {
  std::vector<std::any> ans;
  auto testarray = ctx->test();
  for (const auto &p : testarray) {
    auto val = visit(p);
    ans.push_back(val);
  }
  return ans;
}

std::any EvalVisitor::visitArglist(Python3Parser::ArglistContext *ctx) {
  std::vector<std::any> positional;
  std::vector<std::pair<std::string, std::any>> keyboard;
  auto argumentarray = ctx->argument();
  for (const auto &x : argumentarray) {
    auto p = visit(x);
    if (p.type() == typeid(std::pair<std::string, std::any>)) {
      keyboard.push_back(std::any_cast<std::pair<std::string, std::any>>(p));
    } else {
      positional.push_back(p);
    }
  }
  return std::make_pair(positional, keyboard);
}

std::any EvalVisitor::visitArgument(Python3Parser::ArgumentContext *ctx) {
  auto testarray = ctx->test();
  if (testarray.size() == 1u) {
    auto p = visit(testarray[0]);
    varBack(p);
    return p;
  } else {
    std::string name = std::any_cast<std::pair<std::string, bool>>(visit(testarray[0])).first;
    auto val = visit(testarray[1]);
    varBack(val);
    return std::make_pair(name, val);
  }
}