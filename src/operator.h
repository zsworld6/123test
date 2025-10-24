#pragma once
#ifndef OPERATOR_H
#define OPERATOR_H

#include "int2048.h"
#include <any>
#include <string>

sjtu::int2048 GetInt(const std::any &) ;
std::string GetString(const std::any &);
double GetFlout(const std::any &);
bool GetBool(const std::any &);

std::any operator + (const std::any &);
std::any operator - (const std::any &);

std::any operator + (const std::any &, const std::any &);
std::any operator - (const std::any &, const std::any &);
std::any operator * (const std::any &, const std::any &);
std::any ForceDiv(const std::any &, const std::any &);
std::any operator / (const std::any &, const std::any &);
std::any operator % (const std::any &, const std::any &);

std::any & operator += (std::any &, const std::any &); 
std::any & operator -= (std::any &, const std::any &); 
std::any & operator *= (std::any &, const std::any &); 
std::any & operator /= (std::any &, const std::any &); 
std::any & operator %= (std::any &, const std::any &); 
std::any & ForceDivEqual(std::any &, const std::any &);

bool operator < (const std::any &, const std::any &);
bool operator > (const std::any &, const std::any &);
bool operator <= (const std::any &, const std::any &);
bool operator >= (const std::any &, const std::any &);
bool operator == (const std::any &, const std::any &);
bool operator != (const std::any &, const std::any &);

#endif // OPERATOR_H