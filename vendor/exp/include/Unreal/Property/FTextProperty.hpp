#pragma once

#include <Unreal/FProperty.hpp>
#include <Unreal/TProperty.hpp>
#include <Unreal/FText.hpp>

namespace RC::Unreal
{
    class RC_UE_API FTextProperty : public TProperty<FText, FProperty>
    {
        DECLARE_FIELD_CLASS(FTextProperty);
        DECLARE_VIRTUAL_TYPE(FTextProperty);
    };
}



