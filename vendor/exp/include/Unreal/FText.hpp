#pragma once

#include <string>

#include <Function/Function.hpp>
#include <Unreal/Common.hpp>
#include <Unreal/Core/HAL/Platform.hpp>
#include <Unreal/ITextData.hpp>

namespace RC::Unreal
{
    class FString;

    class RC_UE_API FText
    {
    public:
        ITextData* Data{nullptr};
        void* SharedRefCollector{nullptr};
        uint32_t Flags;
        uint32_t Unk;

    public:
        static Function<FText(const FString&&)> ConstructorInternal;

    private:
        auto construct_with_string(const FString&& String, void* FunctionAddressOverride) -> void
        {
            if (!ConstructorInternal.is_ready() && !FunctionAddressOverride) { return; }

            // Assign the temporary address if one exists
            if (FunctionAddressOverride) { ConstructorInternal.assign_temp_address(FunctionAddressOverride); }

            FText Text = ConstructorInternal(std::forward<const FString>(String));
            Data = Text.Data;
            SharedRefCollector = Text.SharedRefCollector;
            Flags = Text.Flags;
            Unk = Text.Unk;

            // Reset the address to what it was before it was overridden by a temporary address
            if (FunctionAddressOverride) { ConstructorInternal.reset_address(); }
        }

    public:
        FText() = default;

        explicit FText(const FString&& String, void* FunctionAddressOverride = nullptr)
        {
            construct_with_string(std::forward<const FString>(String), FunctionAddressOverride);
        }

        explicit FText(const wchar_t* String, void* FunctionAddressOverride = nullptr)
        {
            FString Str = FString(String);
            construct_with_string(std::forward<const FString>(Str), FunctionAddressOverride);
        }

        explicit FText(StringViewType String, void* FunctionAddressOverride = nullptr) : FText(String.data(), FunctionAddressOverride)
        {
        }

        auto inline operator==(const FString& Other) const -> bool
        {
            return ToFString() == Other;
        }

        auto inline operator==(const wchar_t* Other) const -> bool
        {
            return StringType(ToFString().GetCharArray()) == Other;
        }

        // Beware: Unsafe to call 'ToFString' as 'Data' may be nullptr, call 'ToString' instead.
        auto ToFString() const -> const FString&;
        auto ToString() const -> StringType;

        auto SetString(const FString&&) -> void;
    };
}


