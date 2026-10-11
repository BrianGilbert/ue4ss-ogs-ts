static std::unordered_map<File::StringType, int32_t> MemberOffsets;

public:
    EArrayPropertyFlags& GetArrayFlags();
    const EArrayPropertyFlags& GetArrayFlags() const;

public:
    FProperty*& GetInner();
    const FProperty*& GetInner() const;

