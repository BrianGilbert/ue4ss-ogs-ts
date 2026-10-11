#pragma once

#include <Unreal/FProperty.hpp>

namespace RC::Unreal
{
    class UScriptStruct;

    class RC_UE_API FStructProperty : public FProperty
    {
        DECLARE_FIELD_CLASS(FStructProperty);
        DECLARE_VIRTUAL_TYPE(FStructProperty);

    public:
#include <MemberVariableLayout_HeaderWrapper_FStructProperty.hpp>
    };
}



