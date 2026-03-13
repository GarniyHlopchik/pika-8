#include "json_reader.h"

bool get_json_element_by_key(const std::string &path, const std::string &key, simdjson::dom::element &out){
    simdjson::dom::parser parser;
    simdjson::dom::element element;
    
    auto error = parser.load(path).get(element);
    if (error) {
        std::cerr << "Error loading JSON file: " << error << std::endl;
        return false;
    }

    auto object = element.get_object();
    if (object.error()) {
        std::cerr << "Error parsing JSON: " << object.error() << std::endl;
        return false;
    }

    auto value = object.value()[key];
    if (value.error()) {
        std::cerr << "Key not found: " << key << std::endl;
        return false;
    }

    out = value.value();
    return true;
}