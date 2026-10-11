#pragma once

#include <Unreal/VersionedContainer/Base.hpp>

namespace RC::Unreal
{
    class Derived425 : public Default
    {
    public:
        auto Field_get_type_fname(FField* p_this) -> FName override;
        auto Field_get_ffield_owner(FField* p_this) -> void* override;
        auto FFieldClass_get_fname(FFieldClass* p_this) -> FName override;
    };
}



