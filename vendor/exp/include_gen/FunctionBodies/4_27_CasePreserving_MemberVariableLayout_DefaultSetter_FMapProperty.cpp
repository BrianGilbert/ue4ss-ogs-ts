if (auto it = FMapProperty::MemberOffsets.find(STR("KeyProp")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("KeyProp"), 0x80);
}

if (auto it = FMapProperty::MemberOffsets.find(STR("MapFlags")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("MapFlags"), 0xA8);
}

if (auto it = FMapProperty::MemberOffsets.find(STR("ValueProp")); it == FMapProperty::MemberOffsets.end())
{
    FMapProperty::MemberOffsets.emplace(STR("ValueProp"), 0x88);
}

