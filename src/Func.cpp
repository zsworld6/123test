#include "Func.h"

std::vector<std::pair<std::string, std::any>> Function::GetInitial(const std::string &funcname) {
  return initialval_[funcname];
}

Python3Parser::SuiteContext* Function::GetSuite(const std::string &funcname) {
  return suites_[funcname];
}

void Function::AddFunc(const std::string &name, const std::vector<std::pair<std::string, std::any>> &vals, Python3Parser::SuiteContext* suitecontext) {
  suites_[name] = suitecontext;
  initialval_[name] = vals;
}

