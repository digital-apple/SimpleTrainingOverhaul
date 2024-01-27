#pragma once

#include <RE/Skyrim.h>
#include <SKSE/SKSE.h>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <jsoncons/json.hpp>

#include <xbyak/xbyak.h>

namespace WinAPI = SKSE::WinAPI;
namespace logs = SKSE::log;
using namespace std::literals;

using namespace jsoncons;

namespace stl
{
    using namespace SKSE::stl;

    template <class T>
    void write_thunk_call(std::uintptr_t a_source)
    {
        auto& trampoline = SKSE::GetTrampoline();
        SKSE::AllocTrampoline(14);
        T::function = trampoline.write_call<5>(a_source, T::thunk);
    }
}