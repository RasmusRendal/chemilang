#pragma once
#include "module.h"

class Function : Module {
public:
  Function() {}
  std::string name;
  void VerifyFunction();
};
