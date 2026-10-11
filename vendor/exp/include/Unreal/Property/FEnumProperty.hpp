#pragma once

#include <Unreal/FProperty.hpp>
#include <Unreal/UnrealVersion.hpp>

namespace RC::Unreal
{
    class FNumericProperty;
    class UEnum;

    class RC_UE_API FEnumProperty : public FProperty
    {
        DECLARE_FIELD_CLASS(FEnumProperty);
        DECLARE_VIRTUAL_TYPE(FEnumProperty);

    public:
#include <MemberVariableLayout_HeaderWrapper_FEnumProperty.hpp>

    public:
        auto GetUnderlyingProperty() -> FNumericProperty*;
    };
}



