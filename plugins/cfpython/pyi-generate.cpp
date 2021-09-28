#include <fstream>

extern "C" {
#include "include/cfpython.h"
}

struct methodSignature {
    const char *method;
    const char *parameters;
    const char *type;
    const char *description;
};

struct propertySignature {
    const char *property;
    const char *type;
    const char *description;
};

static methodSignature unknown = { nullptr, nullptr, "any", nullptr };
static propertySignature unknownProperty = { nullptr, "any", nullptr };

static const methodSignature moduleMethods[] = {
    { "FindPlayer", "name: str", "Player", "Find the specified player from its name.\n:param name Player's name, case-sensitive.\n:return Player, None if no player matches." },
    { nullptr, nullptr, nullptr, nullptr },
};

static const methodSignature objectMethods[] = {
    { "WriteKey", "key: str, value: str, add_key: int=0", "bool", "Add a key with the specified value to the object.\n:param key Key name.\n:param value Value to give the key.\n:param add_key If 1 then the key is set, if 0 then the key is only updated if it exists.\n:return True if the key was added or updated, False else." },
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature objectProperties[] = {
    { nullptr, nullptr, nullptr },
};

static const methodSignature playerMethods[] = {
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature playerProperties[] = {
    { "MarkedItem", "Object", "Marked item, used by some actions." },
    { nullptr, nullptr, nullptr },
};

static const methodSignature regionMethods[] = {
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature regionProperties[] = {
    { nullptr, nullptr, nullptr },
};

static const methodSignature mapMethods[] = {
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature mapProperties[] = {
    { nullptr, nullptr, nullptr },
};

static const methodSignature partyMethods[] = {
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature partyProperties[] = {
    { nullptr, nullptr, nullptr },
};

static const methodSignature archMethods[] = {
    { nullptr, nullptr, nullptr, nullptr },
};
static const propertySignature archProperties[] = {
    { nullptr, nullptr, nullptr },
};

const methodSignature *getSignature(const methodSignature *signatures, const char *method) {
    for (size_t s = 0; signatures[s].method; s++) {
        if (strcmp(method, signatures[s].method) == 0) {
            return &signatures[s];
        }
    }
    return &unknown;
}

void writeDescriptionAndBody(std::ofstream &pyi, const char *indent, const char *description) {
    if (description) {
        pyi << std::endl << indent << "\t\"\"\"" << std::endl << description << std::endl << indent << "\t\"\"\"" << std::endl << indent << "..." << std::endl;
    } else {
        pyi << " ..." << std::endl;
    }
}

void writeMethods(std::ofstream &pyi, const PyMethodDef method[], const methodSignature *signatures, bool inClass) {
    const char *indent = inClass ? "\t" : "";
    for (size_t m = 0; method[m].ml_name != nullptr; ++m) {
        auto signature = getSignature(signatures, method[m].ml_name);
        pyi << indent << "def " << method[m].ml_name << "(";
        if (inClass) {
            pyi << "self" << (signature->parameters ? ", " : "");
        };
        if (signature->parameters) {
            pyi << signature->parameters;
        }
        pyi << ") -> " << signature->type << ":";
        writeDescriptionAndBody(pyi, indent, signature->description);
        pyi << std::endl;
    }
}

const propertySignature *getSignature(const propertySignature *properties, const char *property) {
    for (size_t s = 0; properties[s].property; s++) {
        if (strcmp(property, properties[s].property) == 0) {
            return &properties[s];
        }
    }
    return &unknownProperty;
}

void writeClass(std::ofstream &pyi, const char *name, const PyTypeObject *obj, const methodSignature *methods, const propertySignature *properties, const char *base = nullptr) {
    pyi << "class " << name;
    if (base) {
        pyi << "(" << base << ")";
    }
    pyi << ":" << std::endl << std::endl;
    auto property = obj->tp_getset;
    while (property->name) {
        auto signature = getSignature(properties, property->name);
        pyi << "\t@property" << std::endl;
        pyi << "\tdef " << property->name << "(self) -> " << signature->type << ":";
        writeDescriptionAndBody(pyi, "\t", signature->description);
        pyi << std::endl;
        if (property->set) {
            pyi << "\t@" << property->name << ".setter" << std::endl;
            pyi << "\tdef " << property->name << "(self, value: " << signature->type << ") -> None:";
            writeDescriptionAndBody(pyi, "\t", signature->description);
            pyi << std::endl;
        }
        ++property;
    }

    writeMethods(pyi, obj->tp_methods, methods, true);
}

void writeEnum(std::ofstream &pyi, const char *name, const CFConstant *constants) {
    pyi << "class " << name << "(enum):" << std::endl;
    for (size_t c = 0; constants[c].name != nullptr; c++) {
        pyi << "\t" << constants[c].name << " = " << constants[c].value << std::endl;
    }
    pyi << std::endl;
}

void writeConstants(std::ofstream &pyi, const CFConstant *constants) {
    for (size_t c = 0; constants[c].name != nullptr; c++) {
        pyi << constants[c].name << " = " << constants[c].value << std::endl;
    }
    pyi << std::endl;
}

int main(int, char **) {
    std::ofstream pyi("Crossfire.pyi");

    pyi << "\"\"\"" << std::endl << "Crossfire stubs. Auto-generated file, don't edit." << std::endl << "\"\"\"" << std::endl << std::endl;

    writeMethods(pyi, CFPythonMethods, moduleMethods, false);
    writeClass(pyi, "Object", &Crossfire_ObjectType, objectMethods, objectProperties);
    writeClass(pyi, "Player", &Crossfire_PlayerType, playerMethods, playerProperties, "Object");
    writeClass(pyi, "Map", &Crossfire_MapType, mapMethods, mapProperties);
    writeClass(pyi, "Party", &Crossfire_PartyType, partyMethods, partyProperties);
    writeClass(pyi, "Region", &Crossfire_RegionType, regionMethods, regionProperties);
    writeClass(pyi, "Archetype", &Crossfire_ArchetypeType, archMethods, archProperties);

    writeEnum(pyi, "Direction", cstDirection);
    writeEnum(pyi, "Type", cstType);
    writeEnum(pyi, "Move", cstMove);
    writeEnum(pyi, "MessageFlag", cstMessageFlag);
    writeEnum(pyi, "AttackType", cstAttackType);
    writeEnum(pyi, "AttackTypeNumber", cstAttackTypeNumber);
    writeEnum(pyi, "EventType", cstEventType);
    writeConstants(pyi, cstTime);
    writeConstants(pyi, cstReplyTypes);
    writeConstants(pyi, cstAttackMovement);

    pyi << "LogError = 1\nLogInfo = 2\nLogDebug = 3\nLogMonster = 4" << std::endl;

    return 0;
}
