#pragma once
#include "config.h"

unsigned int make_module(unsigned int module_type);
unsigned int make_shader();

// shader declarations
extern const char* vertexSource;
extern const char* fragSource;
