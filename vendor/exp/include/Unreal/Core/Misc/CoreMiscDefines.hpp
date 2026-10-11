// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Unreal/Core/HAL/PreprocessorHelpers.hpp"



// Code analysis features
#ifndef USING_CODE_ANALYSIS
        #define USING_CODE_ANALYSIS 0
#endif

#if USING_CODE_ANALYSIS
        #if !defined( CA_IN ) || !defined( CA_OUT ) || !defined( CA_READ_ONLY ) || !defined( CA_WRITE_ONLY ) || !defined( CA_VALID_POINTER ) || !defined( CA_CHECK_RETVAL ) || !defined( CA_NO_RETURN ) || !defined( CA_SUPPRESS ) || !defined( CA_ASSUME )
                #error Code analysis macros are not configured correctly for this platform
        #endif
#else
        // Just to be safe, define all of the code analysis macros to empty macros
        #define CA_IN 
        #define CA_OUT
        #define CA_READ_ONLY
        #define CA_WRITE_ONLY
        #define CA_VALID_POINTER
        #define CA_CHECK_RETVAL
        #define CA_NO_RETURN
        #define CA_SUPPRESS( WarningNumber )
        #define CA_ASSUME( Expr )
        #define CA_CONSTANT_IF(Condition) if (Condition)
#endif

// Push and pop macro definitions
#ifdef __clang__
        #define PUSH_MACRO(name) _Pragma(PREPROCESSOR_TO_STRING(push_macro(PREPROCESSOR_TO_STRING(name))))
        #define POP_MACRO(name) _Pragma(PREPROCESSOR_TO_STRING(pop_macro(PREPROCESSOR_TO_STRING(name))))
#else
        #define PUSH_MACRO(name) __pragma(push_macro(PREPROCESSOR_TO_STRING(name)))
        #define POP_MACRO(name) __pragma(pop_macro(PREPROCESSOR_TO_STRING(name)))
#endif

/**
 * Macro for marking up code, functions and types deprecated by Unreal Engine in a particular version.
 *
 * This should be used as syntactic replacement for the [[deprecated]] attribute
 * and provides a UE version number.
 *
 * Features that are marked as deprecated are scheduled to be removed from UE's code base
 * in a future release. If you are using a deprecated feature in your code, you should
 * ensure that it still works in all Unreal Engine versions of interest.
 *
 *
 * @param VERSION The release number in which the feature was marked deprecated.
 * @param MESSAGE A message containing upgrade notes.
 */
#define UE_DEPRECATED(Version, Message) [[deprecated(Message " Unreal Engine deprecated this function in " version ". Please ensure this code works in all versions of interest.")]]


/**
* Makes a type non-copyable and non-movable by deleting copy/move constructors and assignment/move operators.
* The macro should be placed in the public section of the type for better compiler diagnostic messages.
* Example usage:
*
*	class FMyClassName
*	{
*	public:
*		UE_NONCOPYABLE(FMyClassName)
*		FMyClassName() = default;
*	};
*/
#define UE_NONCOPYABLE(TypeName) \
  TypeName(TypeName&&) = delete; \
  TypeName(const TypeName&) = delete; \
  TypeName& operator=(const TypeName&) = delete; \
  TypeName& operator=(TypeName&&) = delete;