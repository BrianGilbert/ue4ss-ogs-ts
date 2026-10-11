if (auto it = UDataTable::MemberOffsets.find(STR("RowMap")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("RowMap"), 0x30);
}

if (auto it = UDataTable::MemberOffsets.find(STR("RowStruct")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("RowStruct"), 0x28);
}

if (auto it = UDataTable::MemberOffsets.find(STR("bStripFromClientBuilds")); it == UDataTable::MemberOffsets.end())
{
    UDataTable::MemberOffsets.emplace(STR("bStripFromClientBuilds"), 0x80);
}

