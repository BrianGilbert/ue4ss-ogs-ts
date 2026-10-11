static std::unordered_map<File::StringType, int32_t> MemberOffsets;

public:
    std::function<void(UObject*, void*)>*& GetClassAddReferencedObjects();
    const std::function<void(UObject*, void*)>*& GetClassAddReferencedObjects() const;

public:
    uint64& GetClassCastFlags();
    const uint64& GetClassCastFlags() const;

public:
    FName& GetClassConfigName();
    const FName& GetClassConfigName() const;

public:
    std::function<void(void*)>*& GetClassConstructor();
    const std::function<void(void*)>*& GetClassConstructor() const;

public:
    UObject*& GetClassDefaultObject();
    const UObject*& GetClassDefaultObject() const;

public:
    EClassFlags& GetClassFlags();
    const EClassFlags& GetClassFlags() const;

public:
    UObject*& GetClassGeneratedBy();
    const UObject*& GetClassGeneratedBy() const;

public:
    int32& GetClassUnique();
    const int32& GetClassUnique() const;

public:
    std::function<UObject*(void*)>*& GetClassVTableHelperCtorCaller();
    const std::function<UObject*(void*)>*& GetClassVTableHelperCtorCaller() const;

public:
    UClass*& GetClassWithin();
    const UClass*& GetClassWithin() const;

public:
    int32& GetFirstOwnedClassRep();
    const int32& GetFirstOwnedClassRep() const;

public:
    TMap<FName, UFunction *>& GetFuncMap();
    const TMap<FName, UFunction *>& GetFuncMap() const;

public:
    TMap<FName, UFunction *>& GetInterfaceFuncMap();
    const TMap<FName, UFunction *>& GetInterfaceFuncMap() const;

public:
    TArray<FImplementedInterface,TSizedDefaultAllocator<32> >& GetInterfaces();
    const TArray<FImplementedInterface,TSizedDefaultAllocator<32> >& GetInterfaces() const;

public:
    TArray<UField *,TSizedDefaultAllocator<32> >& GetNetFields();
    const TArray<UField *,TSizedDefaultAllocator<32> >& GetNetFields() const;

public:
    TMap<FName, UFunction *>& GetParentFuncMap();
    const TMap<FName, UFunction *>& GetParentFuncMap() const;

public:
    void*& GetSparseClassData();
    const void*& GetSparseClassData() const;

public:
    UScriptStruct*& GetSparseClassDataStruct();
    const UScriptStruct*& GetSparseClassDataStruct() const;

public:
    TMap<FName, UFunction *>& GetSuperFuncMap();
    const TMap<FName, UFunction *>& GetSuperFuncMap() const;

public:
    FStructProperty*& GetUberGraphFramePointerProperty();
    const FStructProperty*& GetUberGraphFramePointerProperty() const;

public:
    bool& GetbCooked();
    const bool& GetbCooked() const;

public:
    bool& GetbLayoutChanging();
    const bool& GetbLayoutChanging() const;

