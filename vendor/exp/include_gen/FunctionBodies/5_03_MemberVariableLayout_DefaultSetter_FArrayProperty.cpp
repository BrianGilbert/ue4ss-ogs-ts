if (auto it = FArrayProperty::MemberOffsets.find(STR("ArrayFlags")); it == FArrayProperty::MemberOffsets.end())
{
    FArrayProperty::MemberOffsets.emplace(STR("ArrayFlags"), 0x70);
}

if (auto it = FArrayProperty::MemberOffsets.find(STR("Inner")); it == FArrayProperty::MemberOffsets.end())
{
    FArrayProperty::MemberOffsets.emplace(STR("Inner"), 0x78);
}

