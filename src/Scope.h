#pragma once
#ifndef SCPOE_H
#define SCPOE_H

#include <vector>
#include <any>
#include <string>
#include <unordered_map>

class Scope {
private:
  std::vector<std::unordered_map<std::string, std::any>> varmap_{};

public:
  Scope() {
    varmap_.emplace_back();
  }

  void VarRegister(const std::string &varname, std::any vardata) ;
  std::any VarQuery(const std::string varname) ;
  void VarSet(const std::string &varname, std::any vardata) ;
  
  bool VarFind(const std::string &varname) ;

  void LayerCreate() {
    varmap_.emplace_back();
  }
  void LayerDelete() {
    varmap_.pop_back();
  }
};

#endif //SCOPE_H