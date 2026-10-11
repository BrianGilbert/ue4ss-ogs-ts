static std::unordered_map<File::StringType, int32_t> MemberOffsets;

public:
    FString& GetImportKeyField();
    const FString& GetImportKeyField() const;

public:
    TMap<FName, unsigned char *>& GetRowMap();
    const TMap<FName, unsigned char *>& GetRowMap() const;

public:
    TObjectPtr<UScriptStruct>& GetRowStruct();
    const TObjectPtr<UScriptStruct>& GetRowStruct() const;

public:
    uint8& GetbIgnoreExtraFields();
    const uint8& GetbIgnoreExtraFields() const;

public:
    uint8& GetbIgnoreMissingFields();
    const uint8& GetbIgnoreMissingFields() const;

public:
    uint8& GetbStripFromClientBuilds();
    const uint8& GetbStripFromClientBuilds() const;

