if (auto it = UDataTable::MemberOffsets.find(STR("ImportKeyField")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("ImportKeyField"), 0x90);
}

if (auto it = UDataTable::MemberOffsets.find(STR("RowMap")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("RowMap"), 0x38);
}

if (auto it = UDataTable::MemberOffsets.find(STR("RowStruct")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("RowStruct"), 0x30);
}

if (auto it = UDataTable::MemberOffsets.find(STR("bIgnoreExtraFields")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("bIgnoreExtraFields"), 0x88);
}

if (auto it = UDataTable::MemberOffsets.find(STR("bIgnoreMissingFields")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("bIgnoreMissingFields"), 0x88);
}

if (auto it = UDataTable::MemberOffsets.find(STR("bStripFromClientBuilds")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("bStripFromClientBuilds"), 0x88);
}

