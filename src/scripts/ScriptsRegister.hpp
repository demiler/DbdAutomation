#pragma once
#include "../scriptHandler.hpp"
#include "./Wiggle.hpp"
#include "./Autogen.hpp"
#include "./Toxic.hpp"
//move to separte file (smth like, scripts_creation.cpp)
enum class ScriptHandler::Scripts { wiggle, autogen, toxic };

Script* ScriptHandler::createScript(ScriptHandler::Scripts script) {
    switch (script) {
        case Scripts::toxic: return new BecomeToxic;
        case Scripts::autogen: return new Autogen;
        case Scripts::wiggle: return new Wiggle;
        default:
            throw std::invalid_argument("Attempt to create unknown script type");
    }
}