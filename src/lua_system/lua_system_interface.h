#pragma once
class Node;
class LuaSystemInterface{
    public:
        virtual ~LuaSystemInterface() = default;
        virtual void call(const char* name) = 0;
        virtual void call_update(float dt) = 0;
        virtual void call_init(int table_ref, Node& node) = 0;
        virtual void call_script_update(int table_ref,float dt) = 0;
};