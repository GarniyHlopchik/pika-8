#include "lua_diagnostic_proxy.h"
#include "logger/proxy.h"
#include <chrono>
#include <string>

void LuaDiagnosticProxy::call(const char* name){
    auto start = std::chrono::steady_clock::now();

    real_lua.call(name);

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG(LogLevel::DEBUG, std::to_string(diff.count()));
    
}

void LuaDiagnosticProxy::call_update(float dt){
    auto start = std::chrono::steady_clock::now();

    real_lua.call_update(dt);

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG(LogLevel::DEBUG, std::to_string(diff.count()));
}

void LuaDiagnosticProxy::call_init(int table_ref, Node& node){
    auto start = std::chrono::steady_clock::now();

    real_lua.call_init(table_ref, node);

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG(LogLevel::DEBUG, std::to_string(diff.count()));
}
void LuaDiagnosticProxy::call_script_update(int table_ref,float dt){
    auto start = std::chrono::steady_clock::now();

    real_lua.call_script_update(table_ref, dt);

    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    LOG(LogLevel::DEBUG, std::to_string(diff.count()));
}