std::unordered_map<File::StringType, int32_t> FArrayProperty::MemberOffsets{};

EArrayPropertyFlags& FArrayProperty::GetArrayFlags()
{
    static auto offset = MemberOffsets.find(STR("ArrayFlags"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FArrayProperty::ArrayFlags' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<EArrayPropertyFlags*>(this, offset->second);
}
const EArrayPropertyFlags& FArrayProperty::GetArrayFlags() const
{
    static auto offset = MemberOffsets.find(STR("ArrayFlags"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FArrayProperty::ArrayFlags' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const EArrayPropertyFlags*>(this, offset->second);
}

FProperty*& FArrayProperty::GetInner()
{
    static auto offset = MemberOffsets.find(STR("Inner"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FArrayProperty::Inner' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<FProperty**>(this, offset->second);
}
const FProperty*& FArrayProperty::GetInner() const
{
    static auto offset = MemberOffsets.find(STR("Inner"));
    if (offset == MemberOffsets.end()) { throw std::runtime_error{"Tried getting member variable 'FArrayProperty::Inner' that doesn't exist in this engine version."}; }
    return *Helper::Casting::ptr_cast<const FProperty**>(this, offset->second);
}

