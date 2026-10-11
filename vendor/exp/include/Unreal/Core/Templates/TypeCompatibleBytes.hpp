// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


namespace RC::Unreal
{
    /**
     * Used to declare an untyped array of data with compile-time alignment.
     * It needs to use template specialization as the MS_ALIGN and GCC_ALIGN macros require literal parameters.
     */
    template<int32 Size, uint32 Alignment>
    struct TAlignedBytes
    {
        alignas(Alignment) uint8 Pad[Size];
    };

    /** An untyped array of data with compile-time alignment and size derived from another type. */
    template<typename ElementType>
    struct TTypeCompatibleBytes
    {
        using ElementTypeAlias_NatVisHelper = ElementType;
        ElementType*		GetTypedPtr()		{ return (ElementType*)this;  }
        const ElementType*	GetTypedPtr() const	{ return (const ElementType*)this; }

        alignas(ElementType) uint8 Pad[sizeof(ElementType)];
    };
}