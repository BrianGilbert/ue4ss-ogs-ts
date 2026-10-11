#pragma once

#include <Unreal/UObject.hpp>
#include <Unreal/UStruct.hpp>

namespace RC::Unreal
{
    class UGameViewportClient;
    
    struct RC_UE_API FWorldContext
    {
        DECLARE_EXTERNAL_OBJECT_CLASS(FWorldContext, Engine);

    public:
#include <MemberVariableLayout_HeaderWrapper_FWorldContext.hpp>
    };
}
