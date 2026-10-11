std::unordered_map<File::StringType, int32_t> FWorldContext::MemberOffsets{};

uint32& FWorldContext::GetAudioDeviceHandle()
{
    static auto offset = MemberOffsets.find(STR("AudioDeviceHandle"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::AudioDeviceHandle' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint32*>(this, offset->second);
}
const uint32& FWorldContext::GetAudioDeviceHandle() const
{
    static auto offset = MemberOffsets.find(STR("AudioDeviceHandle"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::AudioDeviceHandle' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint32*>(this, offset->second);
}

uint32& FWorldContext::GetAudioDeviceID()
{
    static auto offset = MemberOffsets.find(STR("AudioDeviceID"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::AudioDeviceID' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint32*>(this, offset->second);
}
const uint32& FWorldContext::GetAudioDeviceID() const
{
    static auto offset = MemberOffsets.find(STR("AudioDeviceID"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::AudioDeviceID' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint32*>(this, offset->second);
}

FName& FWorldContext::GetContextHandle()
{
    static auto offset = MemberOffsets.find(STR("ContextHandle"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ContextHandle' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FName*>(this, offset->second);
}
const FName& FWorldContext::GetContextHandle() const
{
    static auto offset = MemberOffsets.find(STR("ContextHandle"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ContextHandle' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FName*>(this, offset->second);
}

FString& FWorldContext::GetCustomDescription()
{
    static auto offset = MemberOffsets.find(STR("CustomDescription"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::CustomDescription' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& FWorldContext::GetCustomDescription() const
{
    static auto offset = MemberOffsets.find(STR("CustomDescription"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::CustomDescription' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >& FWorldContext::GetExternalReferences()
{
    static auto offset = MemberOffsets.find(STR("ExternalReferences"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ExternalReferences' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >*>(this, offset->second);
}
const TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >& FWorldContext::GetExternalReferences() const
{
    static auto offset = MemberOffsets.find(STR("ExternalReferences"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ExternalReferences' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >*>(this, offset->second);
}

TObjectPtr<UGameViewportClient>& FWorldContext::GetGameViewport()
{
    static auto offset = MemberOffsets.find(STR("GameViewport"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::GameViewport' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TObjectPtr<UGameViewportClient>*>(this, offset->second);
}
const TObjectPtr<UGameViewportClient>& FWorldContext::GetGameViewport() const
{
    static auto offset = MemberOffsets.find(STR("GameViewport"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::GameViewport' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TObjectPtr<UGameViewportClient>*>(this, offset->second);
}

TArray<FName,TSizedDefaultAllocator<32> >& FWorldContext::GetLevelsToLoadForPendingMapChange()
{
    static auto offset = MemberOffsets.find(STR("LevelsToLoadForPendingMapChange"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::LevelsToLoadForPendingMapChange' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TArray<FName,TSizedDefaultAllocator<32> >*>(this, offset->second);
}
const TArray<FName,TSizedDefaultAllocator<32> >& FWorldContext::GetLevelsToLoadForPendingMapChange() const
{
    static auto offset = MemberOffsets.find(STR("LevelsToLoadForPendingMapChange"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::LevelsToLoadForPendingMapChange' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TArray<FName,TSizedDefaultAllocator<32> >*>(this, offset->second);
}

float& FWorldContext::GetPIEAccumulatedTickSeconds()
{
    static auto offset = MemberOffsets.find(STR("PIEAccumulatedTickSeconds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEAccumulatedTickSeconds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<float*>(this, offset->second);
}
const float& FWorldContext::GetPIEAccumulatedTickSeconds() const
{
    static auto offset = MemberOffsets.find(STR("PIEAccumulatedTickSeconds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEAccumulatedTickSeconds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const float*>(this, offset->second);
}

float& FWorldContext::GetPIEFixedTickSeconds()
{
    static auto offset = MemberOffsets.find(STR("PIEFixedTickSeconds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEFixedTickSeconds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<float*>(this, offset->second);
}
const float& FWorldContext::GetPIEFixedTickSeconds() const
{
    static auto offset = MemberOffsets.find(STR("PIEFixedTickSeconds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEFixedTickSeconds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const float*>(this, offset->second);
}

int32& FWorldContext::GetPIEInstance()
{
    static auto offset = MemberOffsets.find(STR("PIEInstance"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEInstance' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<int32*>(this, offset->second);
}
const int32& FWorldContext::GetPIEInstance() const
{
    static auto offset = MemberOffsets.find(STR("PIEInstance"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEInstance' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const int32*>(this, offset->second);
}

FString& FWorldContext::GetPIEPrefix()
{
    static auto offset = MemberOffsets.find(STR("PIEPrefix"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEPrefix' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& FWorldContext::GetPIEPrefix() const
{
    static auto offset = MemberOffsets.find(STR("PIEPrefix"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIEPrefix' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

FString& FWorldContext::GetPIERemapPrefix()
{
    static auto offset = MemberOffsets.find(STR("PIERemapPrefix"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIERemapPrefix' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& FWorldContext::GetPIERemapPrefix() const
{
    static auto offset = MemberOffsets.find(STR("PIERemapPrefix"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PIERemapPrefix' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

FString& FWorldContext::GetPendingMapChangeFailureDescription()
{
    static auto offset = MemberOffsets.find(STR("PendingMapChangeFailureDescription"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PendingMapChangeFailureDescription' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& FWorldContext::GetPendingMapChangeFailureDescription() const
{
    static auto offset = MemberOffsets.find(STR("PendingMapChangeFailureDescription"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::PendingMapChangeFailureDescription' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

bool& FWorldContext::GetRunAsDedicated()
{
    static auto offset = MemberOffsets.find(STR("RunAsDedicated"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::RunAsDedicated' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<bool*>(this, offset->second);
}
const bool& FWorldContext::GetRunAsDedicated() const
{
    static auto offset = MemberOffsets.find(STR("RunAsDedicated"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::RunAsDedicated' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const bool*>(this, offset->second);
}

TObjectPtr<UWorld>& FWorldContext::GetThisCurrentWorld()
{
    static auto offset = MemberOffsets.find(STR("ThisCurrentWorld"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ThisCurrentWorld' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TObjectPtr<UWorld>*>(this, offset->second);
}
const TObjectPtr<UWorld>& FWorldContext::GetThisCurrentWorld() const
{
    static auto offset = MemberOffsets.find(STR("ThisCurrentWorld"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::ThisCurrentWorld' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TObjectPtr<UWorld>*>(this, offset->second);
}

uint8& FWorldContext::GetTravelType()
{
    static auto offset = MemberOffsets.find(STR("TravelType"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::TravelType' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint8*>(this, offset->second);
}
const uint8& FWorldContext::GetTravelType() const
{
    static auto offset = MemberOffsets.find(STR("TravelType"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::TravelType' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint8*>(this, offset->second);
}

FString& FWorldContext::GetTravelURL()
{
    static auto offset = MemberOffsets.find(STR("TravelURL"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::TravelURL' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& FWorldContext::GetTravelURL() const
{
    static auto offset = MemberOffsets.find(STR("TravelURL"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::TravelURL' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

bool& FWorldContext::GetbIsPrimaryPIEInstance()
{
    static auto offset = MemberOffsets.find(STR("bIsPrimaryPIEInstance"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bIsPrimaryPIEInstance' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<bool*>(this, offset->second);
}
const bool& FWorldContext::GetbIsPrimaryPIEInstance() const
{
    static auto offset = MemberOffsets.find(STR("bIsPrimaryPIEInstance"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bIsPrimaryPIEInstance' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const bool*>(this, offset->second);
}

uint32& FWorldContext::GetbShouldCommitPendingMapChange()
{
    static auto offset = MemberOffsets.find(STR("bShouldCommitPendingMapChange"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bShouldCommitPendingMapChange' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint32*>(this, offset->second);
}
const uint32& FWorldContext::GetbShouldCommitPendingMapChange() const
{
    static auto offset = MemberOffsets.find(STR("bShouldCommitPendingMapChange"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bShouldCommitPendingMapChange' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint32*>(this, offset->second);
}

bool& FWorldContext::GetbWaitingOnOnlineSubsystem()
{
    static auto offset = MemberOffsets.find(STR("bWaitingOnOnlineSubsystem"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bWaitingOnOnlineSubsystem' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<bool*>(this, offset->second);
}
const bool& FWorldContext::GetbWaitingOnOnlineSubsystem() const
{
    static auto offset = MemberOffsets.find(STR("bWaitingOnOnlineSubsystem"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FWorldContext::bWaitingOnOnlineSubsystem' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const bool*>(this, offset->second);
}

