#pragma once

#include <Unreal/FProperty.hpp>
#include <Unreal/Property/FDelegateProperty.hpp>
#include <Unreal/TArray.hpp>

namespace RC::Unreal
{
    template <typename TWeakPtr = FWeakObjectPtr>
    class TMulticastScriptDelegate
    {
    private:
        using FInvocationList = TArray<TScriptDelegate<TWeakPtr>>;

    public:
        FInvocationList InvocationList;

    public:
        template<typename UObjectTemplate>
        void ProcessMulticastDelegate(void* Parameters) const
        {
            if (InvocationList.Num() > 0)
            {
                using FInlineInvocationList = TArray<TScriptDelegate<TWeakPtr>, TInlineAllocator<4>>;
                FInlineInvocationList InvocationListCopy = FInlineInvocationList(InvocationList);
                for (const auto& InvocationElement : InvocationListCopy)
                {
                    if (InvocationElement.IsBound())
                    {
                        InvocationElement.template ProcessDelegate<UObjectTemplate>(Parameters);
                    }
                }
            }
        }
    };
    using FMulticastScriptDelegate = TMulticastScriptDelegate<>;

    class RC_UE_API FMulticastDelegateProperty : public FProperty
    {
        DECLARE_FIELD_CLASS(FMulticastDelegateProperty);
        DECLARE_VIRTUAL_TYPE(FMulticastDelegateProperty);

    public:
        static std::unordered_map<File::StringType, uint32_t> VTableLayoutMap;

    public:
#include <MemberVariableLayout_HeaderWrapper_FMulticastDelegateProperty.hpp>
        
    // Virtual Functions
    public:
        const FMulticastScriptDelegate* GetMulticastDelegate(const void* PropertyValue) const;
        void SetMulticastDelegate(void* PropertyValue, FMulticastScriptDelegate ScriptDelegate) const;
        void AddDelegate(FScriptDelegate ScriptDelegate, UObject* Parent = nullptr, void* PropertyValue = nullptr) const;
        void RemoveDelegate(const FScriptDelegate& ScriptDelegate, UObject* Parent = nullptr, void* PropertyValue = nullptr) const;
        void ClearDelegate(UObject* Parent = nullptr, void* PropertyValue = nullptr) const;
    };
}


