// CppUserModBase.ABI.hpp — ABI-exact replica of the experimental
// (main-lineage) UE4SS CppUserModBase for the thin experimental build.
//
// Why a replica instead of the real header: the real header transitively
// includes the GUI stack (imgui, LiveView, …) whose headers drift with the
// private UEPseudo dependency. A C++ mod only depends on the class LAYOUT —
// vtable order and member order — plus the imported ctor/dtor. This file
// pins exactly that, nothing else.
//
// Source of truth: RE-UE4SS main @ 5e627997 (the commit the official
// 'experimental-latest' rolling release UE4SS_v3.0.1-1164-g5e627997.zip is
// built from), UE4SS/include/Mod/CppUserModBase.hpp.
// The runtime calls every virtual unconditionally (UE4SS/src/Mod/CppMod.cpp
// fire_on_*), so ALL virtuals must be present, in this exact order — a mod
// built against a shorter vtable gets called out of bounds.
//
// DO NOT add, remove, or reorder virtuals or data members here without
// re-verifying against upstream (see AGENTS.md, experimental track).
#pragma once

#include <memory>
#include <vector>

#include <String/StringType.hpp> // StringType / StringViewType (std::wstring family)

namespace RC
{
    namespace LuaMadeSimple
    {
        class Lua;
    }

    namespace GUI
    {
        class GUITab; // incomplete is sufficient: shared_ptr/vector layouts are type-independent
    }

    class __declspec(dllimport) CppUserModBase
    {
      protected:
        // Declaration order defines object layout — GUITabs comes first.
        std::vector<std::shared_ptr<GUI::GUITab>> GUITabs{};

      public:
        StringType ModName{};
        StringType ModVersion{};
        StringType ModDescription{};
        StringType ModAuthors{};
        StringType ModIntendedSDKVersion{};

      public:
        CppUserModBase();
        virtual ~CppUserModBase();

      public:
        virtual auto on_update() -> void
        {
        }

        // The 'Unreal' module has been initialized.
        virtual auto on_unreal_init() -> void
        {
        }

        // The UI module has been initialized.
        virtual auto on_ui_init() -> void
        {
        }

        virtual auto on_program_start() -> void
        {
        }

        // ---- old hook_luas overloads (v3.0.1 era, kept upstream) ----
        virtual auto on_lua_start(StringViewType mod_name,
                                  LuaMadeSimple::Lua& lua,
                                  LuaMadeSimple::Lua& main_lua,
                                  LuaMadeSimple::Lua& async_lua,
                                  std::vector<LuaMadeSimple::Lua*>& hook_luas) -> void
        {
        }

        virtual auto on_lua_start(LuaMadeSimple::Lua& lua,
                                  LuaMadeSimple::Lua& main_lua,
                                  LuaMadeSimple::Lua& async_lua,
                                  std::vector<LuaMadeSimple::Lua*>& hook_luas) -> void
        {
        }

        virtual auto on_lua_stop(StringViewType mod_name,
                                 LuaMadeSimple::Lua& lua,
                                 LuaMadeSimple::Lua& main_lua,
                                 LuaMadeSimple::Lua& async_lua,
                                 std::vector<LuaMadeSimple::Lua*>& hook_luas) -> void
        {
        }

        virtual auto on_lua_stop(LuaMadeSimple::Lua& lua,
                                 LuaMadeSimple::Lua& main_lua,
                                 LuaMadeSimple::Lua& async_lua,
                                 std::vector<LuaMadeSimple::Lua*>& hook_luas) -> void
        {
        }

        virtual auto on_dll_load(StringViewType dll_name) -> void
        {
        }

        virtual auto render_tab() -> void
        {
        }

        // ---- appended in the main lineage after render_tab (in order) ----
        virtual auto on_lua_start(StringViewType mod_name,
                                  LuaMadeSimple::Lua& lua,
                                  LuaMadeSimple::Lua& main_lua,
                                  LuaMadeSimple::Lua& async_lua,
                                  LuaMadeSimple::Lua* hook_lua) -> void
        {
        }

        virtual auto on_lua_start(LuaMadeSimple::Lua& lua,
                                  LuaMadeSimple::Lua& main_lua,
                                  LuaMadeSimple::Lua& async_lua,
                                  LuaMadeSimple::Lua* hook_lua) -> void
        {
        }

        virtual auto on_lua_stop(StringViewType mod_name,
                                 LuaMadeSimple::Lua& lua,
                                 LuaMadeSimple::Lua& main_lua,
                                 LuaMadeSimple::Lua& async_lua,
                                 LuaMadeSimple::Lua* hook_lua) -> void
        {
        }

        virtual auto on_lua_stop(LuaMadeSimple::Lua& lua,
                                 LuaMadeSimple::Lua& main_lua,
                                 LuaMadeSimple::Lua& async_lua,
                                 LuaMadeSimple::Lua* hook_lua) -> void
        {
        }

        virtual auto on_cpp_mods_loaded() -> void
        {
        }
    };
} // namespace RC
