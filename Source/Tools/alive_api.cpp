#include "alive_api.hpp"
#include "../AliveLibCommon/stdafx_common.h"
#include "../AliveLibAE/Path.hpp"
#include "../AliveLibAE/PathData.hpp"
#include "../AliveLibAE/LvlArchive.hpp"

#include "../AliveLibAO/PathData.hpp"
#include "../AliveLibAO/HoistRocksEffect.hpp"

#include <iostream>
#include "magic_enum.hpp"

#include <jsonxx/jsonxx.h>
#include <gmock/gmock.h>
#include <type_traits>
#include <typeindex>

#define NAME_OF( v ) #v

bool RunningAsInjectedDll()
{
    return false;
}

struct PropertyTypeInfo
{
    std::string mMappedName;
    std::string mTypeName;
};
using TFieldToNameAndTypeMapping = std::map<std::string, PropertyTypeInfo>;


struct CollisionObject
{
    int mX1 = 0;
    int mY1 = 0;
    int mX2 = 0;
    int mY2 = 0;
    // TODO: Other properties

    jsonxx::Object ToJsonObject() const
    {
        jsonxx::Object obj;

        obj << "x1" << mX1;
        obj << "y1" << mY1;

        obj << "x2" << mX2;
        obj << "y2" << mY2;

        return obj;
    }
};

struct ObjectStructureBasicType
{
    std::string mName;
    int mMinValue = 0;
    int mMaxValue = 0;

    void AddToJsonArray(jsonxx::Array& array) const
    {
        jsonxx::Object maxRangeObject;
        maxRangeObject << "min_value" << mMinValue;
        maxRangeObject << "max_value" << mMaxValue;
        array << mName << maxRangeObject;
    }
};

struct ObjectStructurePropertyEnum
{
    std::string mName;
    std::vector<std::string> mValues;

    void AddJsonArray(jsonxx::Object& obj) const
    {
        jsonxx::Array valuesArray;
        for (const auto& value : mValues)
        {
            valuesArray << value;
        }
        obj << mName << valuesArray;
    }
};

struct MapObjectProperty
{
public:
    bool mIsString = false;
    int mIntValue = 0;
    std::string mName;
    std::string mStringValue;

    void AddToJsonObject(jsonxx::Object& obj) const
    {
        if (mIsString)
        {
            obj << mName << mStringValue;
        }
        else
        {
            obj << mName << mIntValue;
        }
    }
};

struct Property
{
    std::string mName;
    std::string mType;
};

struct ObjectStructure
{
    ObjectStructure() = default;
    explicit ObjectStructure(const std::string& name) : mName(name) { }

    std::string mName;
    std::vector<Property> mProperties;

    void AddProps(const TFieldToNameAndTypeMapping& propsToAdd)
    {
        for (const auto& [key, value] : propsToAdd)
        {
            Property property;
            property.mName = value.mMappedName;
            property.mType = value.mTypeName;
            mProperties.push_back(property);
        }
    }

    void AddToJsonArray(jsonxx::Array& array) const
    {
        jsonxx::Object obj;
        obj << "name" << mName;
        jsonxx::Array enumAndBasicTypePropertiesArray;
        for (const auto& property : mProperties)
        {
            obj << property.mName << property.mType;
        }
        obj << "enum_and_basic_type_properties" << enumAndBasicTypePropertiesArray;
        array << obj;
    }

};

struct MapObject
{
    int mXPos = 0;
    int mYPos = 0;
    int mWidth = 0;
    int mHeight = 0;
    std::string mName;
    std::string mObjectStructuresType;
    std::vector<MapObjectProperty> mProperties;

    jsonxx::Object ToJsonObject() const
    {
        jsonxx::Object obj;

        obj << "xpos" << mXPos;
        obj << "ypos" << mYPos;
        obj << "width" << mWidth;
        obj << "height" << mHeight;
        obj << "name" << mName;
        obj << "object_structures_type" << mObjectStructuresType;

        jsonxx::Object propertiesObject;
        for (const auto& property : mProperties)
        {
            property.AddToJsonObject(propertiesObject);
        }
        obj << "properties" << propertiesObject;

        return obj;
    }
};

struct CameraObject
{
    std::string mName;
    int mId = 0;
    int mX = 0;
    int mY = 0;
    std::vector<MapObject> mMapObjects;

    jsonxx::Object ToJsonObject() const
    {
        jsonxx::Object obj;

        obj << "name" << mName;
        obj << "x" << mX;
        obj << "y" << mY;
        obj << "id" << mId;

        jsonxx::Array mapObjectsArray;
        for (const auto& mapObject : mMapObjects)
        {
            mapObjectsArray << mapObject.ToJsonObject();
        }

        obj << "map_objects" << mapObjectsArray;

        return obj;
    }
};



const std::map<std::string, std::string> kTypeNames =
{
    { NAME_OF(Path_Hoist), "Hoist" },
    { NAME_OF(Path_Hoist::Type), "Enum_HoistType" },
    { NAME_OF(Path_Hoist::EdgeType), "Enum_HoistEdgeType" },
    { NAME_OF(Path_Hoist::Scale), "Enum_HoistScale" },

    { NAME_OF(Path_Edge), "Edge" },
    { NAME_OF(Path_Edge::Type), "Enum_EdgeType" },
    { NAME_OF(Path_Edge::Scale), "Enum_EdgeScale" },

};

const std::map<std::string, std::string> kEnum_HoistScale =
{
    { NAME_OF(Path_Hoist::Scale::eFull), "full" },
    { NAME_OF(Path_Hoist::Scale::eHalf), "half" },
};

const std::map<std::string, std::string> kEnum_HoistType =
{
    { NAME_OF(Path_Hoist::Type::eNextFloor), "next_floor" },
    { NAME_OF(Path_Hoist::Type::eNextEdge), "next_edge" },
    { NAME_OF(Path_Hoist::Type::eOffScreen), "off_screen" },
};

const std::map<std::string, std::string> kEnum_HoistEdgeType =
{
    { NAME_OF(Path_Hoist::EdgeType::eLeft), "left" },
    { NAME_OF(Path_Hoist::EdgeType::eRight), "right" },
    { NAME_OF(Path_Hoist::EdgeType::eBoth), "both" },
};

const std::map<std::string, std::string> kEnum_EdgeType =
{
    { NAME_OF(Path_Edge::EdgeType::eLeft), "left" },
    { NAME_OF(Path_Edge::EdgeType::eRight), "right" },
    { NAME_OF(Path_Edge::EdgeType::eBoth), "both" },
};

const std::map<std::string, std::string> kEnum_EdgeScale =
{
    { NAME_OF(Path_Edge::Scale::eFull), "full" },
    { NAME_OF(Path_Edge::Scale::eHalf), "half" },
};

struct StructuresAndTypes
{
    void CreateFromJson(const std::string& /*json*/)
    {

    }

    void CreateCurrentVersion()
    {
        AddBasicTypes();
        Add_Path_Hoist();
        Add_Path_Edge();
    }

    template<class T>
    static std::vector<std::string> AsStringVec(const T& data)
    {
        std::vector<std::string> ret;
        for (const auto& item : data)
        {
            ret.push_back(std::string(magic_enum::enum_name(item)));
        }
        return ret;
    }

    void AddBasicTypes()
    {
        AddBasicType("Bool", 0, 1);
        AddBasicType("Byte", std::numeric_limits<BYTE>::min(), std::numeric_limits<BYTE>::max());
        AddBasicType("Int32", std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        AddBasicType("Int16", std::numeric_limits<short>::min(), std::numeric_limits<short>::max());
    }

    void AddBasicType(const std::string& name, int minVal, int maxVal)
    {
        ObjectStructureBasicType basicType;
        basicType.mName = name;
        basicType.mMinValue = minVal;
        basicType.mMaxValue = maxVal;
        mBasicTypes.push_back(basicType);
    }

    template<class T>
    static std::string Value(const T& map, const std::string& key)
    {
        auto it = map.find(key);
        if (it == std::end(map))
        {
            abort();
        }
        return it->second;
    }

    static std::string TypeName(const std::string& typeName)
    {
        if (typeName == NAME_OF(BYTE))
        {
            return "Byte";
        }
        else if (typeName == NAME_OF(bool))
        {
            return "Bool";
        }
        abort();
    }

    void Add_Path_Hoist()
    {
        const TFieldToNameAndTypeMapping kPath_Hoist =
        {
            { NAME_OF(Path_Hoist::field_10_type), {"HoistType", Value(kTypeNames, NAME_OF(Path_Hoist::Type)) } },
            { NAME_OF(Path_Hoist::field_12_edge_type), {"EdgeType", Value(kTypeNames, NAME_OF(Path_Hoist::EdgeType)) } },
            { NAME_OF(Path_Hoist::field_14_id), {"Id", TypeName(NAME_OF(BYTE)) } },
            { NAME_OF(Path_Hoist::field_16_scale), {"Scale", Value(kTypeNames, NAME_OF(Path_Hoist::Scale)) } },
        };

        auto& type = AddStructure(Value(kTypeNames, NAME_OF(Path_Hoist)));
        type.AddProps(kPath_Hoist);
        AddEnum(Value(kTypeNames, NAME_OF(Path_Hoist::Type)), kEnum_HoistType);
        AddEnum(Value(kTypeNames, NAME_OF(Path_Hoist::EdgeType)), kEnum_HoistEdgeType);
        AddEnum(Value(kTypeNames, NAME_OF(Path_Hoist::Scale)), kEnum_HoistScale);
    }

    void Add_Path_Edge()
    {
        const TFieldToNameAndTypeMapping kPath_Edge =
        {
            { NAME_OF(Path_Edge::field_10_type), {"EdgeType", Value(kTypeNames, NAME_OF(Path_Edge::Type)) } },
            { NAME_OF(Path_Edge::field_12_can_grab), {"CanGrab",  TypeName(NAME_OF(bool)) } },
            { NAME_OF(Path_Edge::field_14_scale), {"Scale", Value(kTypeNames, NAME_OF(Path_Edge::Scale)) } },
        };
        auto& type = AddStructure(Value(kTypeNames, NAME_OF(Path_Edge)));
        type.AddProps(kPath_Edge);
        AddEnum(Value(kTypeNames, NAME_OF(Path_Edge::Type)), kEnum_EdgeType);
        AddEnum(Value(kTypeNames, NAME_OF(Path_Edge::Scale)), kEnum_EdgeScale);
    }


    ObjectStructure& AddStructure(const std::string& typeName)
    {
        ObjectStructure structure;
        structure.mName = typeName;
        mObjectStructures.push_back(structure);

        for (auto& item : mObjectStructures)
        {
            if (item.mName == typeName)
            {
                return item;
            }
        }
        abort();
    }

    void AddEnum(const std::string& enumName, const std::map<std::string, std::string>& values)
    {
        ObjectStructurePropertyEnum newEnum;
        newEnum.mName = enumName;
        for (const auto& [key, value] : values)
        {
            newEnum.mValues.push_back(value);
        }
        mEnums.push_back(newEnum);
    }

    std::vector<ObjectStructure> mObjectStructures;
    std::vector<ObjectStructureBasicType> mBasicTypes;
    std::vector<ObjectStructurePropertyEnum> mEnums;
};

class JsonDocument
{
public:
    int mVersion = 0;
    std::string mGame;

    std::string mPathBnd;
    int mPathId = 0;

    int mXGridSize = 0;
    int mXSize = 0;

    int mYGridSize = 0;
    int mYSize = 0;

    std::vector<CollisionObject> mCollisions;
    std::vector<CameraObject> mCameras;

    StructuresAndTypes mStructureAndTypes;

    void Load(const std::string& fileName)
    {

    }

    void Save(const std::string& fileName) const
    {
        jsonxx::Object rootObject;

        rootObject << "api_version" << mVersion;

        rootObject << "game" << mGame;

        jsonxx::Object rootMapObject;
        rootMapObject << "path_bnd" <<  mPathBnd;
        rootMapObject << "path_id" << mPathId;

        rootMapObject << "x_grid_size" << mXGridSize;
        rootMapObject << "x_size" << mXSize;

        rootMapObject << "y_grid_size" << mYGridSize;
        rootMapObject << "y_size" << mYSize;

        jsonxx::Array collisionsArray;
        for (const auto& item : mCollisions)
        {
            collisionsArray << item.ToJsonObject();
        }
        rootMapObject << "collisions" << collisionsArray;

        jsonxx::Array cameraArray;
        for (const auto& camera : mCameras)
        {
            cameraArray << camera.ToJsonObject();
        }
        rootMapObject << "cameras" << cameraArray;

        jsonxx::Object objectStructurePropertiesObject;
        for (const auto& propertyEnum : mStructureAndTypes.mEnums)
        {
            propertyEnum.AddJsonArray(objectStructurePropertiesObject);
        }
        rootMapObject << "object_structure_property_enums" << objectStructurePropertiesObject; 

        jsonxx::Array objectStructureBasicTypes;
        for (const auto& basicType : mStructureAndTypes.mBasicTypes)
        {
            basicType.AddToJsonArray(objectStructureBasicTypes);
        }
        rootMapObject << "object_structure_property_basic_types" << objectStructureBasicTypes;

        jsonxx::Array objectStructures;
        for (const auto& objectStructure : mStructureAndTypes.mObjectStructures)
        {
            objectStructure.AddToJsonArray(objectStructures);
        }
        rootMapObject << "object_structures" << objectStructures;

        rootObject << "map" << rootMapObject;

        std::ofstream s(fileName.c_str());
        if (s)
        {
            s << rootObject.json();
        }

    }


};

namespace AliveAPI
{
    // Increment when a breaking change to the JSON is made and implement an 
    // upgrade step that converts from the last version to the current.
    constexpr int kApiVersion = 1;

    int GetApiVersion()
    {
        return kApiVersion;
    }

    struct PathInfo
    {
        int mGridWidth = 0;
        int mWidth = 0;
        int mGridHeight = 0;
        int mHeight = 0;
        unsigned int mObjectOffset = 0;
        unsigned int mIndexTableOffset = 0;
    };

    struct PathBND
    {
        Error mResult = Error::None;
        int mNumPaths = 0;
        std::vector<BYTE> mFileData;
        PathInfo mPathInfo;
    };

    [[nodiscard]] static std::vector<unsigned char> ReadLvlFile(LvlArchive& archive, LvlFileRecord& rFileRec)
    {
        std::vector<unsigned char> fileContent;
        fileContent.resize(rFileRec.field_10_num_sectors * 2048);
        sLvlArchive_5BC520.Read_File_4330A0(&rFileRec, fileContent.data());
        fileContent.resize(rFileRec.field_14_file_size);
        return fileContent;
    }

    static PathBND OpenPathBnd(const std::string& inputLvlFile, int* pathId)
    {
        PathBND ret = {};

        // Open the LVL
        LvlArchive lvl;
        if (!lvl.Open_Archive_432E80(inputLvlFile.c_str()))
        {
            ret.mResult = Error::LvlFileReadEror;
            return ret;
        }

        // Find AE Path BND
        {
            for (int i = 0; i < ALIVE_COUNTOF(sPathData_559660.paths); i++)
            {
                auto pathRoot = &sPathData_559660.paths[i];
                if (pathRoot->field_38_bnd_name)
                {
                    LvlFileRecord* pRec = lvl.Find_File_Record_433160(pathRoot->field_38_bnd_name);
                    if (pRec)
                    {
                        ret.mFileData = ReadLvlFile(lvl, *pRec);
                        ret.mResult = Error::None;
                        ret.mNumPaths = pathRoot->field_18_num_paths;
                        if (pathId)
                        {
                            if (*pathId >= 0 && *pathId <= ret.mNumPaths)
                            {
                                const PathBlyRec& pBlyRec = pathRoot->field_0_pBlyArrayPtr[*pathId];
                                if (pBlyRec.field_0_blyName)
                                {
                                    ret.mPathInfo.mHeight = pBlyRec.field_4_pPathData->field_10_height;
                                    ret.mPathInfo.mWidth = pBlyRec.field_4_pPathData->field_E_width;
                                    ret.mPathInfo.mIndexTableOffset = pBlyRec.field_4_pPathData->field_16_object_indextable_offset;
                                    ret.mPathInfo.mObjectOffset = pBlyRec.field_4_pPathData->field_12_object_offset;
                                    return ret;
                                }
                            }
                            ret.mResult = Error::PathResourceNotFound;
                            return ret;
                        }
                        return ret;
                    }
                }
            }
        }

        // Failed, look for AO Path BND
        {
            for (int i = 0; i < ALIVE_COUNTOF(AO::gMapData_4CAB58.paths); i++)
            {
                auto pathRoot = &AO::gMapData_4CAB58.paths[i];
                if (pathRoot->field_38_bnd_name)
                {
                    LvlFileRecord* pRec = lvl.Find_File_Record_433160(pathRoot->field_38_bnd_name);
                    if (pRec)
                    {
                        ret.mFileData = ReadLvlFile(lvl, *pRec);
                        ret.mResult = Error::None;
                        ret.mNumPaths = pathRoot->field_18_num_paths;
                        if (pathId)
                        {
                            if (*pathId >= 0 && *pathId <= ret.mNumPaths)
                            {
                                const AO::PathBlyRec& pBlyRec = pathRoot->field_0_pBlyArrayPtr[*pathId];
                                if (pBlyRec.field_0_blyName)
                                {
                                    ret.mPathInfo.mHeight = pBlyRec.field_4_pPathData->field_A_bBottom;
                                    ret.mPathInfo.mWidth = pBlyRec.field_4_pPathData->field_6_bRight;
                                    ret.mPathInfo.mIndexTableOffset = pBlyRec.field_4_pPathData->field_18_object_index_table_offset;
                                    ret.mPathInfo.mObjectOffset = pBlyRec.field_4_pPathData->field_14_object_offset;
                                }
                            }
                            ret.mResult = Error::PathResourceNotFound;
                            return ret;
                        }
                        return ret;
                    }
                }
            }
        }

        // Both failed
        ret.mResult = Error::PathResourceNotFound;
        return ret;
    }

    Result ExportPathBinaryToJson(const std::string& /*jsonOutputFile*/, const std::string& outputLvlFile, int pathResourceId)
    {
        ResourceManager::Init_49BCE0();
        Result ret = {};
        PathBND pathBnd = OpenPathBnd(outputLvlFile, &pathResourceId);
        ret.mResult = pathBnd.mResult;


        return {};
    }

    Result UpgradePathJson(const std::string& /*jsonFile*/)
    {
        return {};
    }

    Result ImportPathJsonToBinary(const std::string& /*jsonInputFile*/, const std::string& /*inputLvlFile*/, int /*pathResourceId*/, const std::vector<std::string>& /*lvlResourceSources*/)
    {
        return {};
    }

    EnumeratePathsResult EnumeratePaths(const std::string& inputLvlFile)
    {
        ResourceManager::Init_49BCE0();
        EnumeratePathsResult ret = {};
        PathBND pathBnd = OpenPathBnd(inputLvlFile, nullptr);
        ret.mResult = pathBnd.mResult;
        ret.numPaths = pathBnd.mNumPaths;
        return ret;
    }
}

const std::map<std::string, TlvTypes> kObjectNameToAeTlv = 
{
    { "Hoist", TlvTypes::Hoist_2 },
};

const std::map<std::string, AO::TlvTypes> kObjectNameToAoTlv =
{
    { "Hoist", AO::TlvTypes::Hoist_3 },
};

class ITypeBase
{
public:
    explicit ITypeBase(const std::string& typeName) : mName(typeName)
    {

    }

    std::string Name() const
    {
        return mName;
    }

    virtual std::type_index TypeIndex() const = 0;

private:
    std::string mName;
};

template<class T>
class BasicType : public ITypeBase
{
public:
    explicit BasicType(const std::string& typeName, int minVal, int maxVal)
        : ITypeBase(typeName), mTypeIndex(typeid(T)), mMinVal(minVal), mMaxVal(maxVal)
    {

    }

    std::type_index TypeIndex() const override
    {
        return mTypeIndex;
    }

private:
    std::type_index mTypeIndex;
    int mMinVal = 0;
    int mMaxVal = 0;
};

template<class T>
class EnumType : public ITypeBase
{
public:
    explicit EnumType(const std::string& typeName)
        : ITypeBase(typeName), mTypeIndex(typeid(T))
    {

    }

    void Add(T enumValue, const std::string& name)
    {
        mMapping[enumValue] = name;
    }

    std::type_index TypeIndex() const override
    {
        return mTypeIndex;
    }

    T ValueFromString(const std::string& valueString) const
    {
        for (const auto [key, value] : mMapping)
        {
            if (value == valueString)
            {
                return key;
            }
        }
        abort();
    }

    std::string ValueToString(T valueToFind) const
    {
        for (const auto [key, value] : mMapping)
        {
            if (key == valueToFind)
            {
                return value;
            }
        }
        abort();
    }

private:
    std::map<T, std::string> mMapping;
    std::type_index mTypeIndex;
};

class TypesCollection
{
public:
    TypesCollection()
    {
        AddBasicType<BYTE>("Byte", 0, 255);
    }

    std::string TypeName(std::type_index typeIndex) const
    {
        for (const auto& e : mTypes)
        {
            if (e->TypeIndex() == typeIndex)
            {
                return e->Name();
            }
        }
        return "";
    }

    template<class T>
    std::string TypeName() const
    {
        for (const auto& e : mTypes)
        {
            if (e->TypeIndex() == typeid(T))
            {
                return e->Name();
            }
        }
        return "";
    }

    template<class T>
    struct EnumPair
    {
        T mEnumValue;
        std::string mName;
    };

    template<class T>
    auto AddEnum(const std::string& enumName, const std::vector<EnumPair<T>>& enumItems)
    {
        EnumType<T>* ret = nullptr;
        if (!TypeName<T>().empty())
        {
            // Type already exists
            return ret;
        }

        auto newEnum = std::make_unique<EnumType<T>>(enumName);
        for (const auto& enumItem : enumItems)
        {
            newEnum->Add(enumItem.mEnumValue, enumItem.mName);
        }
        ret = newEnum.get();
        mTypes.push_back(std::move(newEnum));
        return ret;
    }

    template<class T>
    T EnumValueFromString(const std::string& enumTypeName, const std::string& enumValueString)
    {
        for (const auto& e : mTypes)
        {
            if (e->Name() == enumTypeName)
            {
               return static_cast<EnumType<T>*>(e.get())->ValueFromString(enumValueString);
            }
        }
        abort();
    }

    template<class T>
    std::string EnumValueToString(T enumValue)
    {
        for (const auto& e : mTypes)
        {
            if (e->TypeIndex() == typeid(T))
            {
                return static_cast<EnumType<T>*>(e.get())->ValueToString(enumValue);
            }
        }
        abort();
    }

    template<class T>
    auto AddBasicType(const std::string& typeName, int minVal, int maxVal)
    {
        BasicType<T>* ret = nullptr;
        if (!TypeName<T>().empty())
        {
            // Type already exists
            return ret;
        }

        auto newType = std::make_unique<BasicType<T>>(typeName, minVal, maxVal);
        ret = newType.get();
        mTypes.push_back(std::move(newType));
        return ret;
    }


private:
    std::vector<std::unique_ptr<ITypeBase>> mTypes;
};

class IMapObject
{
public:
    explicit IMapObject(const std::string& typeName)
        : mName(typeName)
    {

    }

    virtual ~IMapObject() {}

    std::string Name() const
    {
        return mName;
    }

    void AddProperty(const std::string& name, const std::string& typeName, void* key)
    {
        mInfo[key] = { name, typeName };
    }

    std::string PropName(void* key) const
    {
        auto it = mInfo.find(key);
        if (it == std::end(mInfo))
        {
            abort();
        }
        return it->second.mName;
    }

    std::string PropType(void* key) const
    {
        auto it = mInfo.find(key);
        if (it == std::end(mInfo))
        {
            abort();
        }
        return it->second.mTypeName;
    }

    template<class T>
    void ReadEnumValue(TypesCollection& types, T& field, jsonxx::Object& properties)
    {
        const std::string propName = PropName(&field);
        const std::string propType = PropType(&field);
        const std::string jsonValue = properties.get<std::string>(propName);
        field = types.EnumValueFromString<T>(propType, jsonValue);
    }

    template<class T>
    void WriteEnumValue(TypesCollection& types, jsonxx::Object& properties, T& field)
    {
        properties << PropName(&field) << types.EnumValueToString<T>(field);
    }

    template<class T>
    void ReadBasicType(T& field, jsonxx::Object& properties)
    {
        field = properties.get<T>(PropName(&field));
    }

    template<class T>
    void WriteBasicType(T& field, jsonxx::Object& properties)
    {
        properties << properties.get<T>(PropName(&field));
    }

private:
    struct PropertyInfo
    {
        std::string mName;
        std::string mTypeName;
    };
    std::map<void*, PropertyInfo> mInfo;
    std::string mName;
protected:
    std::string mDescription;
};

#define ADD_PROP(name, type) AddProperty(name, globalTypes.TypeName(typeid(type)), &type);

namespace Editor
{
    class Path_Hoist : public IMapObject
    {
    public:
        Path_Hoist(TypesCollection& globalTypes) : IMapObject("Hoist")
        {
            globalTypes.AddEnum<::Path_Hoist::Type>("Enum_HoistType",
                {
                    {::Path_Hoist::Type::eNextEdge, "next_edge"},
                    {::Path_Hoist::Type::eNextFloor, "next_floor"},
                    {::Path_Hoist::Type::eOffScreen, "off_screen"},
                });

            globalTypes.AddEnum<::Path_Hoist::EdgeType>("Enum_HoistEdgeType",
                {
                    {::Path_Hoist::EdgeType::eBoth, "both"},
                    {::Path_Hoist::EdgeType::eLeft, "left"},
                    {::Path_Hoist::EdgeType::eRight, "right"},
                });

            globalTypes.AddEnum<::Path_Hoist::Scale>("Enum_HoistScale", 
                {
                    {::Path_Hoist::Scale::eFull, "full"},
                    {::Path_Hoist::Scale::eHalf, "half"}
                });

            ADD_PROP("HoistType", mData.field_10_type);
            ADD_PROP("HoistEdgeType", mData.field_12_edge_type);
            ADD_PROP("Id", mData.field_14_id);
            ADD_PROP("Scale", mData.field_16_scale);
        }

        void InstanceFromJsonBase(jsonxx::Object& obj)
        {
            mDescription = obj.get<std::string>("name");

            mData.field_8_top_left.field_0_x = obj.get<int>("xpos");
            mData.field_8_top_left.field_2_y = obj.get<int>("ypos");
            mData.field_C_bottom_right.field_0_x = obj.get<int>("width");
            mData.field_C_bottom_right.field_2_y = obj.get<int>("height");
        }

        void InstanceFromJson(TypesCollection& types, jsonxx::Object& obj)
        {
            InstanceFromJsonBase(obj);

            jsonxx::Object properties = obj.get<jsonxx::Object>("properties");

            ReadEnumValue(types, mData.field_10_type, properties);
            ReadEnumValue(types, mData.field_12_edge_type, properties);
            ReadBasicType(mData.field_14_id, properties);
            ReadEnumValue(types, mData.field_16_scale, properties);

        }

        void InstanceToJsonBase(jsonxx::Object& ret)
        {
            ret << "name" << mDescription;

            ret << "xpos" << mData.field_8_top_left.field_0_x;
            ret << "ypos" << mData.field_8_top_left.field_2_y;
            ret << "width" << mData.field_C_bottom_right.field_0_x;
            ret << "height" << mData.field_C_bottom_right.field_2_y;

            ret << "object_structures_type" << Name();

        }

        jsonxx::Object InstanceToJson(TypesCollection& types)
        {
            jsonxx::Object ret;

            InstanceToJsonBase(ret);

            jsonxx::Object properties;

            WriteEnumValue(types, properties, mData.field_10_type);
            WriteEnumValue(types, properties, mData.field_12_edge_type);
            WriteBasicType(mData.field_14_id, properties);
            WriteEnumValue(types, properties, mData.field_16_scale);

            ret << "properties" << properties;

            return ret;
        }



    private:
        ::Path_Hoist mData = {};
    };
}

int main(int argc, char* argv[])
{
    TypesCollection globalTypes;
    Editor::Path_Hoist eph(globalTypes);
    auto obj = eph.InstanceToJson(globalTypes);
    auto v = obj.json();
    LOG_INFO(v);
    eph.InstanceFromJson(globalTypes, obj);


    // Create "fixed" structure data
    StructuresAndTypes structuresAndTypes;
    structuresAndTypes.CreateCurrentVersion();

    JsonDocument doc;
    doc.mStructureAndTypes.CreateCurrentVersion();

    CameraObject testCam;
    testCam.mName = "lol cam";
    testCam.mId = 77;

    MapObject testMapObject;
    testMapObject.mName = "Hoisty mc hoist face";
    testMapObject.mObjectStructuresType = "Hoist";
    testMapObject.mWidth = 20;
    testMapObject.mHeight = 40;
    testMapObject.mXPos = 999;
    testMapObject.mYPos = 1234;

    MapObjectProperty testPropertyInt;
    testPropertyInt.mName = "Id";
    testPropertyInt.mIntValue = 7;
    testPropertyInt.mIsString = false;
    testMapObject.mProperties.push_back(testPropertyInt);

    MapObjectProperty testPropertyString;
    testPropertyString.mName = "Scale";
    testPropertyString.mStringValue = "Full";
    testPropertyString.mIsString = false;
    testMapObject.mProperties.push_back(testPropertyString);


    testCam.mMapObjects.push_back(testMapObject);
    doc.mCameras.push_back(testCam);

    doc.Save("lol.json");
    
    JsonDocument doc2;
    doc2.Load("lol.json");

    // Read a json file and use the "fixed" data to decode it

    // Compile the json data to a binary path resource

   // AliveAPI::EnumeratePaths("C:\\GOG Games\\Abes Exoddus\\MI.LVL");

    AliveAPI::ExportPathBinaryToJson("Output.json", "C:\\GOG Games\\Abes Exoddus\\MI.LVL", 1);



    return 0;
}