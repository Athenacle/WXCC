#include "semantic/type.h"

#include "utils/error_manager.h"
#include "utils/utils.h"


USING_V2
using namespace std;
using namespace semantic;
using namespace utils;

BaseType::~BaseType() {}

Type::Type()
{
    base_.clear();
}

Type::~Type() {}

void Type::addBaseType(TypeOperator op)
{
    /*
    BaseType** t = &base_;

    for (; *t && (*t)->op_ != TypeOperator::TO_NONE; t = &((*t)->next_)) {
    }

    if (*t == nullptr) {
        *t = new BaseType(op);
    } else {
        (*t)->op_ = op;
    }

    if (*t == reinterpret_cast<BaseType*>(0x100000003)) {
        assert(false);
    }

    static auto cc = [this]() {
        BaseType** t = &base_;

        for (; *t && (*t)->op_ != TypeOperator::TO_NONE; t = &((*t)->next_)) {
            if (*t == reinterpret_cast<BaseType*>(0x100000003)) {
                break;
            }
        }
    };
    cc();*/

    base_.push_back(op);
}

vector<tuple<int, string>> Type::checkType() const
{
    if (empty()) {
        return {};
    }

    vector<tuple<int, string>> ret;

    auto mask = 0u;

    for (const auto& t : base_) {
        mask = mask | static_cast<uint32_t>(t.operator_);
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
