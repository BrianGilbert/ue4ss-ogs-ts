#pragma once

#include <unordered_map>

#include <Unreal/Core/HAL/Platform.hpp>

namespace RC::Unreal
{
    // Non-existant structs.
    struct FCustomPropertyListNode {};
    struct FCustomVersionContainer {};
    struct FEngineVersionBase {};
    struct FPackageFileVersion {};
    class ITargetPlatform {};
    
    // Forward declarations.
    class FProperty;

    struct FArchiveState
    {
    public:
        static std::unordered_map<File::StringType, uint32_t> VTableLayoutMap;

    public:
#include <MemberVariableLayout_HeaderWrapper_FArchiveState.hpp>
        
    };

    struct FArchive : FArchiveState
    {
    public:
        static std::unordered_map<File::StringType, uint32_t> VTableLayoutMap;

    public:
#include <MemberVariableLayout_HeaderWrapper_FArchive.hpp>
        
    };
}


