#pragma once
#include "config.h"
#include "simdjson.h"
#include <optional>

struct FontData;

bool get_json_element_by_key(const std::string &path, const std::string &key, simdjson::dom::element &out);