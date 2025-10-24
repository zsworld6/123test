#include "InnerFunc.h"
#include "Evalvisitor.h"
#include "int2048.h"
#include "operator.h"
#include <any>
#include <cassert>
#include <cstddef>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

bool CheckInner(const std::string &s) {
  return s == "print" || s == "int" || s == "float" || s == "str" || s == "bool";
}

void PrintVal(std::any val, char ch) {
  if (val.type() == typeid(sjtu::int2048)) {
    std::cout << std::any_cast<sjtu::int2048>(val) << ch;
  } else if (val.type() == typeid(std::string)) {
    auto s = std::any_cast<std::string>(val);
    for (int i = 0; i < s.size(); i++) {
      if (s[i] == '\\') {
        i++;
        if (i >= s.size()) {
          break;
        }
        if (s[i] == 'n') {
          std::cout << '\n';
        } else if (s[i] == 't') {
          std::cout << '\t';
        } else if (s[i] == '\"') {
          std::cout << '\"';
        } else if (s[i] == '\'') {
          std::cout << '\'';
        } else {
          std::cout << s[i];
        }
      } else {
        std::cout << s[i];
      }
    }
    std::cout << ch;
  } else if (val.type() == typeid(double)) {
    std::cout << std::fixed << std::setprecision(6) << std::any_cast<double>(val) << ch;
  } else if (val.type() == typeid(bool)) {
    std::cout << (std::any_cast<bool>(val) ? "True" : "False") << ch;
  } else if (!val.has_value()){
    std::cout << "None" << ch;
  } else if (val.type() == typeid(std::vector<std::any>)){
    auto array = std::any_cast<std::vector<std::any>>(val);
    for (size_t i = 0; i + 1 < array.size(); i++) {
      PrintVal(array[i], ' ');
    }
    PrintVal(array.back(), ch);
  }
}

std::any Inner(const std::string &funcname, const std::vector<std::any> &val) {
  // std::cerr << "INNER!!!!!!!!!!!!\n";
  if (funcname == "print") {
    // std::cerr << "Print!!\n";
    for (size_t i = 0; i < val.size(); i++) {
      if (i + 1 < val.size()) {
        PrintVal(val[i], ' ');
      } else {
        PrintVal(val[i], '\n');
      }
    }
    if (val.empty()) {
      std::cout << "\n";
    }
    return kNOTFLOW;
  } else if (funcname == "int") {
    return GetInt(val[0]);
  } else if (funcname == "str") {
    return GetString(val[0]);
  } else if (funcname == "bool") {
    return GetBool(val[0]);
  } else if (funcname == "float") {
    return GetFlout(val[0]);
  } else {
    assert("INVALID INNER FUNC" == 0);
    return std::any();
  }
}