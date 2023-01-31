#include "semantic/type.h"

#include <unordered_map>

#include "utils/error_manager.h"
#include "utils/utils.h"


USING_V2
using namespace std;
using namespace semantic;
using namespace utils;
using namespace lex::constants;

std::vector<uint32_t> possibleTypeSpecifiers = {
    maskOr(TypeOperator::TO_VOID),  // void

    maskOr(TypeOperator::TO_CHAR),                             // char
    maskOr(TypeOperator::TO_SIGNED, TypeOperator::TO_CHAR),    // signed char
    maskOr(TypeOperator::TO_UNSIGNED, TypeOperator::TO_CHAR),  // unsigned char

    maskOr(TypeOperator::TO_SHORT,
           TypeOperator::TO_SIGNED,
           TypeOperator::TO_INT),  // short, signed char, short int, signed short int

    maskOr(TypeOperator::TO_UNSIGNED,
           TypeOperator::TO_SHORT,
           TypeOperator::TO_INT),  // unsigned short, unsigned short int

    maskOr(TypeOperator::TO_INT, TypeOperator::TO_SIGNED),  // int, signed int
};

// simple type
Type::Type()
{
    base_.clear();
}

Type::~Type() {}

void Type::addBaseType(TypeOperator op)
{
    base_.push_back(op);
}


bool Type::compare(const BaseType&) const
{
    return false;
}

BaseType::TypeAssignmentCheckReturnType Type::assign(const BaseType&) const
{
    return BaseType::TypeAssignmentCheckReturnType();
}

string Type::print() const
{
    static unordered_map<uint32_t, string> typeMap = {{}};

    vector<string> parts;
    if (bits_.test<Q_CONST>()) {
        parts.emplace_back("const");
    }
    if (bits_.test<Q_VOLATILE>()) {
        parts.emplace_back("volatile");
    }
    if (bits_.test<Q_RESTRICT>()) {
        parts.emplace_back("restrict");
    }

    return join(parts, " ");
}

vector<tuple<int, string>> Type::checkType() const
{
    if (empty()) {
        return {};
    }

    vector<tuple<int, string>> ret;

    auto mask = 0u;

    for (const auto& t : base_) {
        mask = mask | static_cast<uint32_t>(t);
    }


    static vector<tuple<uint32_t, string, string>> conflicts = {
        {maskOr(TypeOperator::TO_SIGNED, TypeOperator::TO_UNSIGNED), "signed", "unsigned"},

        {maskOr(TypeOperator::TO_CHAR, TypeOperator::TO_INT), "char", "int"},
        {maskOr(TypeOperator::TO_CHAR, TypeOperator::TO_LONG), "char", "long"},
        {maskOr(TypeOperator::TO_CHAR, TypeOperator::TO_SHORT), "char", "short"},
        {maskOr(TypeOperator::TO_CHAR, TypeOperator::TO_DOUBLE), "char", "double"},
        {maskOr(TypeOperator::TO_CHAR, TypeOperator::TO_FLOAT), "char", "float"},

        {maskOr(TypeOperator::TO_LONG, TypeOperator::TO_SHORT), "long", "short"},
        {maskOr(TypeOperator::TO_LONG, TypeOperator::TO_FLOAT), "long", "float"},

        {maskOr(TypeOperator::TO_DOUBLE, TypeOperator::TO_SHORT), "double", "short"},
        {maskOr(TypeOperator::TO_FLOAT, TypeOperator::TO_SHORT), "float", "short"},

        {maskOr(TypeOperator::TO_DOUBLE, TypeOperator::TO_INT), "double", "int"},
        {maskOr(TypeOperator::TO_FLOAT, TypeOperator::TO_INT), "float", "int"},

        {maskOr(TypeOperator::TO_DOUBLE, TypeOperator::TO_FLOAT), "double", "float"}};


    for (const auto& c : conflicts) {
        const auto& [m, ct1, ct2] = c;
        if (maskMatch(mask, m)) {
            ret.emplace_back(
                utils::ErrorManager::ERROR,
                fmt::format(FMT("both '{}' and '{}' in declaration specifiers"), ct1, ct2));
        }
    }

    return ret;
}

KEYWORD Type::findQualifier() const
{
    assert(!bits_.none());
    if (bits_.test<Q_AUTO>()) {
        return KEY_AUTO;
    }
    if (bits_.test<Q_EXTERN>()) {
        return KEY_EXTERN;
    }
    if (bits_.test<Q_REGISTER>()) {
        return KEY_REGISTER;
    }
    if (bits_.test<Q_STATIC>()) {
        return KEY_STATIC;
    }
    if (bits_.test<Q_TYPEDEF>()) {
        return KEY_TYPEDEF;
    }
    if (bits_.test<Q_CONST>()) {
        return KEY_CONST;
    }
    if (bits_.test<Q_VOLATILE>()) {
        return KEY_VOLATILE;
    }
    // if (t.test<Type::Q_RESTRICT>()){
    // }
    UNREACHABLE
    return KEY_NONE;
}
