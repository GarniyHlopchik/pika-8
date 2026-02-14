#include "json_reader.h"

simdjson::dom::element get_json_element_by_key(const std::string &path, const std::string &key){
    simdjson::dom::parser parser;
    simdjson::dom::element element;
    auto error = parser.load(path).get(element);
    if (error) {
        std::cerr << "Error loading JSON file: " << error << std::endl;
        return simdjson::dom::element(); // Return an invalid element
    }

    auto object = element.get_object();
    if (object.error()) {
        std::cerr << "Error parsing JSON: " << object.error() << std::endl;
        return simdjson::dom::element(); // Return an invalid element
    }

    auto value = object.value()[key];
    if (value.error()) {
        std::cerr << "Key not found: " << key << std::endl;
        return simdjson::dom::element(); // Return an invalid element
    }

    return value.value();
}