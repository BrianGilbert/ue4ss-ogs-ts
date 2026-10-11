#pragma once

namespace RC::Unreal
{
    class UObject;

    // Simple class that makes everything compile.
    // Add onto this class helper functionality to make it behave like an actual pointer.
    template<typename UnderlyingType>
    class TObjectPtr
    {
    public:
        UnderlyingType* UnderlyingObjectPointer;
    };

    // TODO: Move to its own file.
    template<class TClass>
    class TSubclassOf
    {
    public:
        TClass* Class;
    };
}

