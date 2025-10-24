#pragma once
#ifndef INNERFUNC_H
#define INNERFUNC_H

#include <any>
#include <string>
#include <vector>

bool CheckInner(const std::string &);

std::any Inner(const std::string &, const std::vector<std::any> &);

#endif // INNERFUNC_H