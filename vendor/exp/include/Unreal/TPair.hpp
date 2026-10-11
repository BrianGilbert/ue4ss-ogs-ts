#pragma once

namespace RC::Unreal
{
    template<typename KeyType, typename ValueType>
    struct TTuple
    {
        KeyType Key;
        ValueType Value;

        bool operator==(const TTuple& Other) const noexcept
        {
            return Key == Other.Key && Value == Other.Value;
        }
    };

    template<typename KeyType, typename ValueType>
    using TPair = TTuple<KeyType, ValueType>;
}


