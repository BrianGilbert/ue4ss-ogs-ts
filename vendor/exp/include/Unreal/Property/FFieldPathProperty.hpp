#pragma once

#include <Unreal/FProperty.hpp>
#include <Unreal/TProperty.hpp>
#include <Unreal/FWeakObjectPtr.hpp>
#include <Unreal/NameTypes.hpp>
#include <Unreal/TArray.hpp>
#include <Unreal/FieldPath.hpp>

namespace RC::Unreal
{
    class RC_UE_API FFieldPathProperty : public TProperty<FFieldPath, FProperty>
    {
        DECLARE_FIELD_CLASS(FFieldPathProperty);
        DECLARE_VIRTUAL_TYPE(FFieldPathProperty);

    public:
#include <MemberVariableLayout_HeaderWrapper_FFieldPathProperty.hpp>
    };
}



