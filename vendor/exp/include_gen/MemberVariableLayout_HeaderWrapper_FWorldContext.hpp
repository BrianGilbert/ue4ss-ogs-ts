static std::unordered_map<File::StringType, int32_t> MemberOffsets;

public:
    uint32& GetAudioDeviceHandle();
    const uint32& GetAudioDeviceHandle() const;

public:
    uint32& GetAudioDeviceID();
    const uint32& GetAudioDeviceID() const;

public:
    FName& GetContextHandle();
    const FName& GetContextHandle() const;

public:
    FString& GetCustomDescription();
    const FString& GetCustomDescription() const;

public:
    TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >& GetExternalReferences();
    const TArray<TObjectPtr<UWorld> *,TSizedDefaultAllocator<32> >& GetExternalReferences() const;

public:
    TObjectPtr<UGameViewportClient>& GetGameViewport();
    const TObjectPtr<UGameViewportClient>& GetGameViewport() const;

public:
    TArray<FName,TSizedDefaultAllocator<32> >& GetLevelsToLoadForPendingMapChange();
    const TArray<FName,TSizedDefaultAllocator<32> >& GetLevelsToLoadForPendingMapChange() const;

public:
    float& GetPIEAccumulatedTickSeconds();
    const float& GetPIEAccumulatedTickSeconds() const;

public:
    float& GetPIEFixedTickSeconds();
    const float& GetPIEFixedTickSeconds() const;

public:
    int32& GetPIEInstance();
    const int32& GetPIEInstance() const;

public:
    FString& GetPIEPrefix();
    const FString& GetPIEPrefix() const;

public:
    FString& GetPIERemapPrefix();
    const FString& GetPIERemapPrefix() const;

public:
    FString& GetPendingMapChangeFailureDescription();
    const FString& GetPendingMapChangeFailureDescription() const;

public:
    bool& GetRunAsDedicated();
    const bool& GetRunAsDedicated() const;

public:
    TObjectPtr<UWorld>& GetThisCurrentWorld();
    const TObjectPtr<UWorld>& GetThisCurrentWorld() const;

public:
    uint8& GetTravelType();
    const uint8& GetTravelType() const;

public:
    FString& GetTravelURL();
    const FString& GetTravelURL() const;

public:
    bool& GetbIsPrimaryPIEInstance();
    const bool& GetbIsPrimaryPIEInstance() const;

public:
    uint32& GetbShouldCommitPendingMapChange();
    const uint32& GetbShouldCommitPendingMapChange() const;

public:
    bool& GetbWaitingOnOnlineSubsystem();
    const bool& GetbWaitingOnOnlineSubsystem() const;

