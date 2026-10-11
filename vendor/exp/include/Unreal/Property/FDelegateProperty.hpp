#pragma once

#include <Helpers/Format.hpp>

#include <Unreal/UObject.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/TProperty.hpp>
#include <Unreal/FWeakObjectPtr.hpp>

namespace RC::Unreal
{
    class UFunction;

    template <typename TWeakPtr = FWeakObjectPtr>
    class TScriptDelegate
    {
    public:
        TWeakPtr Object;
        FName FunctionName;

    public:
        template<typename UObjectTemplate>
        void ProcessDelegate(void* Parameters) const
        {
            auto ObjectPtr = static_cast<UObjectTemplate*>(Object.Get());
            auto Function = ObjectPtr->GetFunctionByName(FunctionName);
            ObjectPtr->ProcessEvent(Function, Parameters);
        }

        template<typename UObjectTemplate>
        bool IsBound_Internal() const
        {
            if (FunctionName != NAME_None)
            {
                if (UObject* ObjectPtr = Object.Get())
                {
                    // Here, we're supposed to cast to 'UObjectTemplate*' instead of assuming it's UObject*.
                    return ObjectPtr->GetFunctionByName(FunctionName) != nullptr;
                }
            }
            return false;
        }


        bool IsBound() const
        {
            return IsBound_Internal<UObject>();
        }
    };
    using FScriptDelegate = TScriptDelegate<>;

    class RC_UE_API FDelegateProperty : public TProperty<FScriptDelegate, FProperty>
    {
        DECLARE_FIELD_CLASS(FDelegateProperty);
        DECLARE_VIRTUAL_TYPE(FDelegateProperty);

    public:
#include <MemberVariableLayout_HeaderWrapper_FDelegateProperty.hpp>
        
    };
}



