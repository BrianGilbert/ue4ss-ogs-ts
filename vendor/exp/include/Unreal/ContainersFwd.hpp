#pragma once

namespace RC::Unreal
{
    template<int IndexSize>
    class TSizedDefaultAllocator;
    using FDefaultAllocator = TSizedDefaultAllocator<32>;

    template<typename T, typename Allocator = FDefaultAllocator>
    class TArray;
}


