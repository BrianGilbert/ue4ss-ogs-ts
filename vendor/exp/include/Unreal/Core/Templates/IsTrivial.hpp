// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Unreal/Core/Templates/AndOrNot.h"
#include <Unreal/Core/Templates/IsTriviallyDestructible.h"
#include <Unreal/Core/Templates/IsTriviallyCopyConstructible.h"
#include <Unreal/Core/Templates/IsTriviallyCopyAssignable.h"

namespace RC::Unreal
{
    /**
     * Traits class which tests if a type is trivial.
     */
    template <typename T>
    struct TIsTrivial
    {
        enum { Value = TAnd<TIsTriviallyDestructible<T>, TIsTriviallyCopyConstructible<T>, TIsTriviallyCopyAssignable<T>>::Value };
    };
}