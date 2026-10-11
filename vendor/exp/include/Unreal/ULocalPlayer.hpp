#pragma once

#include <Unreal/UPlayer.hpp>
#include <Unreal/TObjectPtr.hpp>
#include <Unreal/UGameViewportClient.hpp>
#include <Unreal/UnrealCoreStructs.hpp>

// patched (MSVC build): the generated layout wrappers below reference this
// engine enum via an elaborated-type-specifier that MSVC rejects as a first
// declaration; forward-declare it with a fixed underlying type instead.
enum EAspectRatioAxisConstraint : int;

namespace RC::Unreal
{
    // TODO: Move to its own file.
    template<typename TEnum>
    class TEnumAsByte
    {
    private:
        uint8_t Value;

    public:
        TEnumAsByte() = default;
        TEnumAsByte(TEnum) {};
    };

    class RC_UE_API ULocalPlayer : public UPlayer
    {
        DECLARE_EXTERNAL_OBJECT_CLASS(ULocalPlayer, Engine);

    public:
        static std::unordered_map<File::StringType, uint32_t> VTableLayoutMap;

    public:
        using ExecSignature = bool(ULocalPlayer* Context, UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar);
        static Function<ExecSignature> ExecInternal;

    public:
#include <MemberVariableLayout_HeaderWrapper_ULocalPlayer.hpp>
    };
}



