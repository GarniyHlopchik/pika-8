#pragma once
#include "config.h"
#include "simdjson.h"
#include <optional>

bool get_json_element_by_key(const std::string &path, const std::string &key, simdjson::dom::element &out);