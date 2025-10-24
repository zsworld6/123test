#include "operator.h"
#include "int2048.h"
#include <any>
#include <cassert>
#include <iostream>
#include <string>

sjtu::int2048 GetInt(const std::any &a) {
  if (a.type() == typeid(sjtu::int2048)) {
    return std::any_cast<sjtu::int2048>(a);
  } else if (a.type() == typeid(std::string)) {
    return sjtu::int2048(std::any_cast<std::string>(a));
  } else if (a.type() == typeid(bool)) {
    return sjtu::int2048(std::any_cast<bool>(a) ? 1 : 0);
  } else if (a.type() == typeid(double)){
    return sjtu::int2048((long long)(std::any_cast<double>(a)));
  } else {
    return sjtu::int2048(0);
  }
}
bool GetBool(const std::any &a) {
  if (a.type() == typeid(bool)) {
    return std::any_cast<bool>(a);
  } else if (a.type() == typeid(std::string)) {
    return std::any_cast<std::string>(a) != "";
  } else if (a.type() == typeid(sjtu::int2048)) {
    return std::any_cast<sjtu::int2048>(a) != sjtu::int2048(0);
  } else if (a.type() == typeid(double)) {
    return std::any_cast<double>(a) != 0;
  } else {
    return false;
  }
}
double GetFlout(const std::any &a) {
  if (a.type() == typeid(bool)) {
    return std::any_cast<bool>(a) ? 1.0 : 0.0;
  } else if (a.type() == typeid(std::string)) {
    return stod(std::any_cast<std::string>(a));
  } else if (a.type() == typeid(sjtu::int2048)) {
    return double(std::any_cast<sjtu::int2048>(a));
  } else if (a.type() == typeid(double)) {
    return std::any_cast<double>(a);
  } else {
    return 0.0;
  }
}
std::string GetString(const std::any &a) {
  if (a.type() == typeid(bool)) {
    return std::any_cast<bool>(a) ? "True" : "False";
  } else if (a.type() == typeid(std::string)) {
    return std::any_cast<std::string>(a);
  } else if (a.type() == typeid(sjtu::int2048)) {
    return std::string(std::any_cast<sjtu::int2048>(a));
  } else if (a.type() == typeid(double)) {
    return std::to_string(std::any_cast<double>(a));
  } else if (!a.has_value()) {
    return std::string("None");
  } else {
    return "";
  }
} 

std::any operator + (const std::any &a) {
  return a;
}
std::any operator - (const std::any &a) {
  if (a.type() == typeid(std::string)) {
    // throw invalid operator
    std::cerr << "test " << std::any_cast<std::string>(a) << "\n";
    assert("invalid operator" == 0);
  }
  if (a.type() == typeid(double)) {
    return -*std::any_cast<double>(&a);
  } 
  return -*std::any_cast<sjtu::int2048>(&a);
}

std::any operator + (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    return GetString(a) + GetString(b);
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) + GetFlout(b);
  }
  return GetInt(a) + GetInt(b);
}
std::any operator - (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    // throw unvalid operation
    assert("unvalid operation" == 0);
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) - GetFlout(b);
  }
  return GetInt(a) - GetInt(b);
}
std::any operator * (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) && b.type() == typeid(std::string)) {
    // throw unvalid operation
    assert("unvalid operation" == 0);
  }
  if (a.type() == typeid(std::string)) {
    std::string s = GetString(a);
    sjtu::int2048 t = GetInt(b);
    std::string ans = "";
    for (sjtu::int2048 i = 0; i < t; i += 1) {
      ans = ans + s;
    }
    return ans;
  }
  if (b.type() == typeid(std::string)) {
    std::string s = GetString(b);
    sjtu::int2048 t = GetInt(a);
    std::string ans = "";
    for (sjtu::int2048 i = 0; i < t; i += 1) {
      ans = ans + s;
    }
    return ans;
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) * GetFlout(b);
  }
  return GetInt(a) * GetInt(b);
}
std::any ForceDiv(const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    // throw unvalid operation
    assert("unvalid operation" == 0);
  }
  std::any res = GetInt(a) / GetInt(b);
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(res);
  }
  return res;
}
std::any operator / (const std::any &a, const std::any &b)  {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    // throw unvalid operation
    assert("unvalid operation" == 0);
  }
  return GetFlout(a) / GetFlout(b);
}
std::any operator % (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    // throw unvalid operation
    assert("unvalid operation" == 0);
  }
  return a - b * ForceDiv(a, b);
}

std::any & operator += (std::any &a, const std::any &b) {
  a = a + b;
  return a;
}
std::any & operator -= (std::any &a, const std::any &b) {
  a = a - b;
  return a;
}
std::any & operator *= (std::any &a, const std::any &b) {
  a = a * b;
  return a;
}
std::any & operator /= (std::any &a, const std::any &b) {
  a = a / b;
  return a;
}
std::any & operator %= (std::any &a, const std::any &b) {
  a = a % b;
  return a;
} 
std::any & ForceDivEqual(std::any &a, const std::any &b) {
  a = ForceDiv(a, b);
  return a;
}

bool operator < (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    if (a.type() != b.type()) {
      return false;
    }
    return std::any_cast<std::string>(a) < std::any_cast<std::string>(b);
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) < GetFlout(b);
  }
  return GetInt(a) < GetInt(b);
}
bool operator > (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    if (a.type() != b.type()) {
      return false;
    }
    return std::any_cast<std::string>(a) > std::any_cast<std::string>(b);
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) > GetFlout(b);
  }
  return GetInt(a) > GetInt(b);
}
bool operator <= (const std::any &a, const std::any &b) {
  return !(a > b);
}
bool operator >= (const std::any &a, const std::any &b) {
  return !(a < b);
}
bool operator == (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    if (a.type() != b.type()) {
      return false;
    }
    return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
  }
  if (!a.has_value() || !b.has_value()) {
    return a.type() == b.type();
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) == GetFlout(b);
  }
  return GetInt(a) == GetInt(b);
}
bool operator != (const std::any &a, const std::any &b) {
  if (a.type() == typeid(std::string) || b.type() == typeid(std::string)) {
    if (a.type() != b.type()) {
      return false;
    }
    return std::any_cast<std::string>(a) != std::any_cast<std::string>(b);
  }
  if (!a.has_value() || !b.has_value()) {
    return a.type() != b.type();
  }
  if (a.type() == typeid(double) || b.type() == typeid(double)) {
    return GetFlout(a) != GetFlout(b);
  }
  return GetInt(a) != GetInt(b);
}
