std::unordered_map<File::StringType, int32_t> UDataTable::MemberOffsets{};

FString& UDataTable::GetImportKeyField()
{
    static auto offset = MemberOffsets.find(STR("ImportKeyField"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::ImportKeyField' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FString*>(this, offset->second);
}
const FString& UDataTable::GetImportKeyField() const
{
    static auto offset = MemberOffsets.find(STR("ImportKeyField"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::ImportKeyField' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FString*>(this, offset->second);
}

TMap<FName, unsigned char *>& UDataTable::GetRowMap()
{
    static auto offset = MemberOffsets.find(STR("RowMap"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::RowMap' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TMap<FName, unsigned char *>*>(this, offset->second);
}
const TMap<FName, unsigned char *>& UDataTable::GetRowMap() const
{
    static auto offset = MemberOffsets.find(STR("RowMap"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::RowMap' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TMap<FName, unsigned char *>*>(this, offset->second);
}

TObjectPtr<UScriptStruct>& UDataTable::GetRowStruct()
{
    static auto offset = MemberOffsets.find(STR("RowStruct"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::RowStruct' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<TObjectPtr<UScriptStruct>*>(this, offset->second);
}
const TObjectPtr<UScriptStruct>& UDataTable::GetRowStruct() const
{
    static auto offset = MemberOffsets.find(STR("RowStruct"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::RowStruct' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const TObjectPtr<UScriptStruct>*>(this, offset->second);
}

uint8& UDataTable::GetbIgnoreExtraFields()
{
    static auto offset = MemberOffsets.find(STR("bIgnoreExtraFields"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bIgnoreExtraFields' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint8*>(this, offset->second);
}
const uint8& UDataTable::GetbIgnoreExtraFields() const
{
    static auto offset = MemberOffsets.find(STR("bIgnoreExtraFields"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bIgnoreExtraFields' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint8*>(this, offset->second);
}

uint8& UDataTable::GetbIgnoreMissingFields()
{
    static auto offset = MemberOffsets.find(STR("bIgnoreMissingFields"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bIgnoreMissingFields' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint8*>(this, offset->second);
}
const uint8& UDataTable::GetbIgnoreMissingFields() const
{
    static auto offset = MemberOffsets.find(STR("bIgnoreMissingFields"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bIgnoreMissingFields' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint8*>(this, offset->second);
}

uint8& UDataTable::GetbStripFromClientBuilds()
{
    static auto offset = MemberOffsets.find(STR("bStripFromClientBuilds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bStripFromClientBuilds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<uint8*>(this, offset->second);
}
const uint8& UDataTable::GetbStripFromClientBuilds() const
{
    static auto offset = MemberOffsets.find(STR("bStripFromClientBuilds"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'UDataTable::bStripFromClientBuilds' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const uint8*>(this, offset->second);
}

