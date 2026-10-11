if (auto it = FMapProperty::MemberOffsets.find(STR("KeyProp")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("KeyProp"), 0x78);
}

if (auto it = FMapProperty::MemberOffsets.find(STR("MapFlags")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("MapFlags"), 0xA0);
}

if (auto it = FMapProperty::MemberOffsets.find(STR("ValueProp")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("ValueProp"), 0x80);
}

