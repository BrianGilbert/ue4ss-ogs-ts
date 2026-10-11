#pragma once

#include <format>
#include <vector>

#include <Constructs/Generator.hpp>
#include <Unreal/Common.hpp>
#include <Unreal/FField.hpp>
#include <Unreal/TypeChecker.hpp>
#include <Unreal/UField.hpp>
#include <Unreal/TArray.hpp>
#include <Unreal/TPair.hpp>
#include <Unreal/FText.hpp>
#include <Unreal/UnrealFlags.hpp>

namespace RC::Unreal
{
    // The "UEnum::Names" pair is "TPair<FName, uint8>" in earlier engine versions
    // This can cause a problem, therefore, you should be careful when using this pair
    // This is a case where we perhaps should abstract this array and deny direct access to the TArray
    using FEnumNamePair = TPair<FName, int64_t>;
    using FEnumNameArray = TArray<FEnumNamePair>;

    class RC_UE_API UEnum : public UField
    {
        DECLARE_EXTERNAL_OBJECT_CLASS(UEnum, CoreUObject)

    public:
        enum class ECppForm
        {
            /**
             * Regular enums are declared using the normal enum keyword
             * Their constants are unscoped and usually prefixed with enum name
             */
            Regular,
            /**
             * Namespaced enums use the namespace for enum name + enum keyword for type
             * They are scoped to their namespace
             */
            Namespaced,
            /**
             * Enum classes are C++11 enum class enumerations that are properly scoped
             * They can also specify any integral underlying type
             */
            EnumClass
        };

    public:
#include <MemberVariableLayout_HeaderWrapper_UEnum.hpp>

    private:
        TArray<TPair<FName, uint8>>& GetEnumNamesOldUnsafe();
        TArray<TPair<FName, int64>>& GetEnumNamesNewUnsafe();

    public:
        /**
         * Returns the array of enumeration constants and their values
         * Keep in mind that enumeration can have gaps as well as negative values
         * The last entry of the array will always be a _MAX constant
         */
        auto GetEnumNames() -> TArray<TPair<FName, int64>>;
        auto GetEnumNameByIndex(int32 Index) -> TPair<FName, int64>;
        void GetEnumNamesAsVector(std::vector<std::pair<FName, int64>>& OutNames);

        /**
         * Returns the flags set on the enumeration object
         * Will return EEnumFlags::None on older UE versions
         */
        auto GetEnumFlags() -> EEnumFlags;

        auto ForEachName() -> Generator<TPair<FName, int64>>
        {
            if (Version::IsBelow(4, 15))
            {
                for (TPair<FName, uint8>& Elem : GetEnumNamesOldUnsafe()) {
                    co_yield TPair<FName, int64>{Elem.Key, Elem.Value};
                }
            }
            else
            {
                for (TPair<FName, int64>& Elem : GetEnumNamesNewUnsafe()) {
                    co_yield Elem;
                }
            }
        }

        /**
        * Gets the name for the enumerator with the input value, if found.
        * 
        * @param Value of the enum to find.
        *
        * @return Name of the enumerator, if found, as an FName.
        */
        FName GetNameByValue(int64 Value);

        /**
        * @return   The number of enumerators.
        */
        int32 NumEnums();

        /**
        * Find the longest common prefix of all items in the enumeration.
        * 
        * @return   the longest common prefix between all items in the enum.  If a common prefix
        *           cannot be found, returns the full name of the enum.
        */
        StringType GenerateEnumPrefix();

        
        /**
        * Edits the name of the enumerator at the given location in the names array.
        * 
        * @note Wrapper around the TArray method to handle UEnum names array changing types
        * after UE 4.14
        *
        * @param Index Location in array of the element to edit.
        * @param NewName New name as an FName.
        */
        void EditNameAt(int32 Index, FName NewName);
        
        /**
        * Edits the value of the enumerator at the given location in the names array.
        * 
        * @note Wrapper around the TArray method to handle UEnum names array changing types
        * after UE 4.14
        *
        * @param Index Location in array of the element to edit.
        * @param NewValue New value to use.
        */
        void EditValueAt(int32 Index, int64 NewValue);
        
        /**
         * Removes an element (or elements) at given location in UEnum names array, 
         * optionally shrinking the array.
         *
         * @note Wrapper around TArray::RemoveAt to handle UEnum names array changing types
         * after UE 4.14
         *
         * @param Index Location in array of the element to remove.
         * @param Count (Optional) Number of elements to remove. Default is 1.
         * @param bAllowShrinking (Optional) Tells if this call can shrink array if suitable after remove. Default is true.
         */
        void RemoveFromNamesAt(int32 Index, int32 Count = 1, bool bAllowShrinking = true);

        /**
         * 
         * Inserts a given element into the UEnum names array at given location. Move semantics
         * version.  Optionally shifts all enum values greater than inserted value by one.
         *
         * @note Wrapper around TArray::RemoveAt to handle UEnum names array changing types
         * after UE 4.14
         *
         * @param Item The element to insert.
         * @param Index Tells where to insert the new elements.
         * @param bShiftValues (Optional) Tells if this call should shift enum values greater than the inserted value. Default is false.
         * @returns Location at which the insert was done, or -1 if failed.
         * 
         */
        int32 InsertIntoNames(const TPair<FName, int64>& Item, int32 Index, bool bShiftValues = false);
        

        /**
         * Retrieves the enumeration index associated with the provided enum name
         * Returns INDEX_NONE if there is no enumeration constant with the provided name
         */
        //auto GetIndexByName(const std::wstring& name) -> int32_t;

        //auto GetValueByName(const std::wstring& name) -> int64_t;
    };

    class RC_UE_API UUserDefinedEnum : public UEnum
    {
        DECLARE_EXTERNAL_OBJECT_CLASS(UUserDefinedEnum, Engine);
    };
}



