#pragma once
#include "config.h"
#include "simdjson.h"

simdjson::dom::element get_json_element_by_key(const std::string &path, const std::string &key);