#pragma once
#include "config.h"
#include "gfx/gfx_struct.h"

unsigned int make_module(unsigned int module_type);
unsigned int make_shader();

unsigned int make_debug_module(unsigned int module_type);
unsigned int make_debug_shader();

// shader declarations
extern const char* vertexSource;
extern const char* fragSource;

extern const char* debugVertexSource;
extern const char* debugFragSource;
