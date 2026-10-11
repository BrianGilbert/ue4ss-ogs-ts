#pragma once

#include <unordered_map>

#include <Function/Function.hpp>
#include <Unreal/Common.hpp>
#include <Unreal/Core/HAL/Platform.hpp>

/*
 * How GMalloc gets created:
 * 1. UnrealMemory.cpp -> FMemory_GCreateMalloc_ThreadUnsafe()
 * 2.     It sets GMalloc to:
 * 3. WindowsPlatformMemory.cpp -> FWindowsPlatformMemory::BaseAllocator()
 * 4. It selects Binned2
 * 5. It uses 'new' to create an instance of 'FMallocBinned2' and returns it
 */

namespace RC::Unreal
{
    struct FGenericMemoryStats;

    namespace UnrealInitializer
    {
        struct Config;
        struct CacheInfo;
        RC_UE_API auto CreateCache(UnrealInitializer::CacheInfo& Target) -> void;
        RC_UE_API auto LoadCache(UnrealInitializer::CacheInfo& Target) -> void;
    }

    namespace Signatures
    {
        struct ScanResult;
        RC_UE_API auto ScanForGUObjectArrayImpl(const UnrealInitializer::Config&) -> ScanResult;
    }

    class RC_UE_API FMalloc
    {
    public:
        static std::unordered_map<File::StringType, uint32_t> VTableLayoutMap;

    private:
        friend Signatures::ScanResult Signatures::ScanForGUObjectArrayImpl(const UnrealInitializer::Config&);
        friend RC_UE_API void UnrealInitializer::CreateCache(UnrealInitializer::CacheInfo&);
        friend RC_UE_API void UnrealInitializer::LoadCache(UnrealInitializer::CacheInfo&);

    public:
        static FMalloc** UnrealStaticGMalloc;

    public:
        void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        void* TryMalloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        void* Realloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        void* TryRealloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        void Free(void* Original);
        SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment);
        bool GetAllocationSize(void* Original, SIZE_T& SizeOut);
        void Trim(bool bTrimThreadCaches);
        void SetupTLSCachesOnCurrentThread();
        void ClearAndDisableTLSCachesOnCurrentThread();
        void InitializeStatsMetadata();
        void UpdateStats();
        void GetAllocatorStats(FGenericMemoryStats& out_Stats);
        void DumpAllocatorStats(class FOutputDevice& Ar);
        bool IsInternallyThreadSafe() const;
        bool ValidateHeap();
        const TCHAR* GetDescriptiveName();
    };

    RC_UE_API extern FMalloc* GMalloc;

    class RC_UE_API FMemory
    {
    public:
        //static void* Memmove(void* Dest, const void* Src, SIZE_T Count);
        static void* Malloc(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        static void* Realloc(void* Original, SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);
        static void Free(void* Original);

        static SIZE_T QuantizeSize(SIZE_T Count, uint32 Alignment = DEFAULT_ALIGNMENT);

        static FORCEINLINE void* Memmove( void* Dest, const void* Src, SIZE_T Count )
        {
            return memmove( Dest, Src, Count );
        }

        static FORCEINLINE int32 Memcmp( const void* Buf1, const void* Buf2, SIZE_T Count )
        {
            return memcmp( Buf1, Buf2, Count );
        }

        static FORCEINLINE void* Memset(void* Dest, uint8 Char, SIZE_T Count)
        {
            return memset( Dest, Char, Count );
        }

        static FORCEINLINE void* Memzero(void* Dest, SIZE_T Count)
        {
            return memset( Dest, 0, Count );
        }

        static FORCEINLINE void* Memcpy(void* Dest, const void* Src, SIZE_T Count)
        {
            return memcpy( Dest, Src, Count );
        }

        /** Memcpy optimized for big blocks. */
        static FORCEINLINE void* BigBlockMemcpy(void* Dest, const void* Src, SIZE_T Count)
        {
            return memcpy(Dest, Src, Count);
        }

        /** On some platforms memcpy optimized for big blocks that avoid L2 cache pollution are available */
        static FORCEINLINE void* StreamingMemcpy(void* Dest, const void* Src, SIZE_T Count)
        {
            return memcpy( Dest, Src, Count );
        }

    private:
        template <typename T>
        static FORCEINLINE void Valswap(T& A, T& B)
        {
            // Usually such an implementation would use move semantics, but
            // we're only ever going to call it on fundamental types and MoveTemp
            // is not necessarily in scope here anyway, so we don't want to
            // #include it if we don't need to.
            T Tmp = A;
            A = B;
            B = Tmp;
        }

        static void MemswapGreaterThan8( void* Ptr1, void* Ptr2, SIZE_T Size );

    public:
        static inline void Memswap( void* Ptr1, void* Ptr2, SIZE_T Size )
        {
            switch (Size)
            {
                case 0:
                    break;

                case 1:
                    Valswap(*(uint8*)Ptr1, *(uint8*)Ptr2);
                break;

                case 2:
                    Valswap(*(uint16*)Ptr1, *(uint16*)Ptr2);
                break;

                case 3:
                    Valswap(*((uint16*&)Ptr1)++, *((uint16*&)Ptr2)++);
                Valswap(*(uint8*)Ptr1, *(uint8*)Ptr2);
                break;

                case 4:
                    Valswap(*(uint32*)Ptr1, *(uint32*)Ptr2);
                break;

                case 5:
                    Valswap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
                Valswap(*(uint8*)Ptr1, *(uint8*)Ptr2);
                break;

                case 6:
                    Valswap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
                Valswap(*(uint16*)Ptr1, *(uint16*)Ptr2);
                break;

                case 7:
                    Valswap(*((uint32*&)Ptr1)++, *((uint32*&)Ptr2)++);
                Valswap(*((uint16*&)Ptr1)++, *((uint16*&)Ptr2)++);
                Valswap(*(uint8*)Ptr1, *(uint8*)Ptr2);
                break;

                case 8:
                    Valswap(*(uint64*)Ptr1, *(uint64*)Ptr2);
                break;

                case 16:
                    Valswap(((uint64*)Ptr1)[0], ((uint64*)Ptr2)[0]);
                Valswap(((uint64*)Ptr1)[1], ((uint64*)Ptr2)[1]);
                break;

                default:
                    MemswapGreaterThan8(Ptr1, Ptr2, Size);
                break;
            }
        }
    };
}


