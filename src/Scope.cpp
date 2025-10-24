#include "Scope.h"

void Scope::VarRegister(const std::string &varname, std::any vardata) {
  varmap_.back()[varname] = vardata;
}

std::any Scope::VarQuery(const std::string varname) {
  if (varmap_.back().find(varname) != varmap_.back().end()) {
    return varmap_.back()[varname];
  } else {
    return varmap_[0][varname];
  }
}

void Scope::VarSet(const std::string &varname, std::any vardata) {
  if (!varmap_.back().empty() &&
      varmap_.back().find(varname) != varmap_.back().end()) {
    varmap_.back()[varname] = vardata;
  } else if (varmap_[0].find(varname) != varmap_[0].end()){
    varmap_[0][varname] = vardata;
  } else {
    varmap_.back()[varname] = vardata;
  }
}

bool Scope::VarFind(const std::string &varname) {
  return (varmap_.back().find(varname) != varmap_.back().end()) ||
         (varmap_[0].find(varname) != varmap_[0].end());
}