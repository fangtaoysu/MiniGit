#pragma once

#include <string>
#include <vector>

#include "shared/model.h"

class Validator {
public:
    virtual bool Validate(const LexicalResult& cmd) = 0;
};