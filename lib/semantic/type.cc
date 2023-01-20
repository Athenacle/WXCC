#include "semantic/type.h"

NAMESPACE_BEGIN

using namespace semantic;

void Type::addBaseType(TypeOperator op)
{
    auto* t = &base_;
    for (; t->op_ != TypeOperator::TO_NONE; t = t->base_) {
    }
    assert(t->op_ == TypeOperator::TO_NONE);

    t->op_ = op;
}

NAMESPACE_END
