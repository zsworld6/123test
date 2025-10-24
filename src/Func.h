#include "Python3Parser.h"
#include <unordered_map>
#include <utility>

class Function {
private:
  std::unordered_map<std::string, Python3Parser::SuiteContext*> suites_;
  std::unordered_map<std::string, std::vector<std::pair<std::string, std::any>>> initialval_{};
public:
  std::vector<std::pair<std::string, std::any>> GetInitial(const std::string &) ;
  Python3Parser::SuiteContext* GetSuite(const std::string &) ;
  Function () {}
  void AddFunc(const std::string &, const std::vector<std::pair<std::string, std::any>> &, Python3Parser::SuiteContext*);
};