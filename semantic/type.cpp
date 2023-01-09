/* type check and other functions
* WangXiaochun
* zjjhwxc(at)gmail.com
* Mar 8, 2013
*
*/

#include "type.h"
#include "tools.h"
#include "semantic.h"
#include "exception.h"

//using namespace NS_SEMACTIC;
using namespace NS_TYPE_OP;

#define TYPE_ERR_TOO_TYPES    1
#define TYPE_ERR_BOTH_USIGNED 2

#define TYPE_WARN_USFN        1


std::map<NS_TYPE_OP::TYPE_OPERATOR, const char *> Type::to2c;

namespace
{
    const int TE_FUNC_RET_FUNC = 3;
    const int TE_FUNC_RET_ARR = 4;
    const int TE_FUNC_NEED_FUNC = 5;
    const int TE_ARR_CONT_FUNC = 6;
    const char *typeError[] = {
        "NULL",

        "TE01: Too many types.\n",

        "TE02: Both unsigned and signed?\n",

        "TE03. Line %3d: Function '%s' cannot return a function.\n",

        "TE04. Line %3d: Function '%s' cannot return an array.\n",

        "TE05. Line %3d: Function cannot be a parameter of another function.\n",

        "TE06: Line %3d: Array cannot contain functions.\n"};
    const char *typeWarning[] = {"NULL",

                                 "TW01: floating type with a signed/unsigned specifier. ignore.\n"};
};  // namespace

Type::Type(TYPE_OPERATOR _oper,
           Type *_baseType,
           int _arrEls,
           Type *_list,
           int _size,
           int _align,
           void *_symbol)
{
    this->sym = NULL;
    this->name = NULL;
    type_op = _oper;
    size = _size;
    base_type = _baseType;
    u.a.nElements = 0;
    u.f.paraList = NULL;
    if (_oper == TO_CHAR)
        size = 1;
    else if (_oper == TO_DOUBLE)
        size = 4;
    else if (_oper == TO_ARRAY) {
        size = _arrEls * _baseType->size;
        u.a.nElements = _arrEls;
    } else if (_oper == TO_FUNCTION) {
        size = NS_TYPE_OP::NO_LIMIT;
        u.f.paraList = _list;
    }
    align = _align;
    sym = static_cast<Symbol *>(_symbol);
}

const TypeException &Type::checkType(const Type &ty, TypeException &te)
{
    if (ty.type_op == TO_FUNCTION)
        te = checkFunction(ty, te);

    if (ty.type_op == TO_ARRAY)
        if (ty.base_type->type_op == TO_FUNCTION) { /* asm {int 3} */
        }
    //TODO: an array whose element is functions.

    int meetSigned = 0;
    int meetUSigned = 0;
    int meetBaseType = 0;
    int meetFloat = 0;

    const Type *tpp = &ty;
    for (; tpp != NULL; tpp = tpp->base_type) {
        if (matchBaseType(*tpp))
            meetBaseType++;
        if (tpp->type_op == TO_SIGNED)
            meetSigned++;
        if (tpp->type_op == TO_UNSIGNED)
            meetUSigned++;
        if (tpp->type_op == TO_DOUBLE || tpp->type_op == TO_FLOAT)
            meetFloat++;
    }
    if (meetBaseType > 1)
        te.setError(typeError[TYPE_ERR_TOO_TYPES]);
    else if ((meetUSigned > 0 && meetSigned > 0))
        te.setError(typeError[TYPE_ERR_BOTH_USIGNED]);
    else if (meetSigned + meetUSigned >= 2 && meetFloat >= 0)
        te.setError(typeError[TYPE_WARN_USFN]);

    return te;
}

void Type::print(void)
{
    char buf[100] = {0};
    print(buf);
    name = strdup(buf);
}

char *Type::print(char *buffer)
{
    if (this == NULL)
        return buffer;
    if (type_op == TO_FUNCTION) {
        Type *ytp = this->u.f.paraList;
        char *ptr = this->base_type->print(buffer);
        ptr += sprintf(ptr, " (*)(");
        ptr = u.f.paraList->print(ptr);
        return ptr + sprintf(ptr, ").");
    } else {
        if (type_op == TO_POINTER) {
            if (base_type->type_op == TO_ARRAY) {
                char *ptr = buffer;
                ptr = base_type->base_type->print(ptr);
                return ptr
                       + sprintf(ptr,
                                 "%s (*)[%d] ",
                                 base_type->base_type->print(ptr),
                                 base_type->u.a.nElements);
            } else if (base_type->type_op == TO_FUNCTION) {
                char *ptr = buffer;
                ptr = this->base_type->print(ptr);
                //ptr += sprintf(ptr, "(*)(");
                Type *ty = this->u.f.paraList;
            paraPrint:
                ptr = ty->print(ptr);
                if (ty != NULL) {
                    ty = ty->u.f.paraList;
                    goto paraPrint;
                }

            } else {
                char *ptr = base_type->print(buffer);
                return ptr + sprintf(ptr, " * ");
            }
        } else if (type_op == TO_ARRAY) {
            char *ptr = base_type->print(buffer);
            return ptr + sprintf(ptr, "[%d]", u.a.nElements);
        } else if (type_op == TO_FUNCPARA) {
            char *ptr = base_type->print(buffer);

            if (u.f.paraList != NULL) {
                ptr += sprintf(ptr, ", ");
                return u.f.paraList->print(ptr);
            } else
                return ptr;
        } else {
            return buffer + sprintf(buffer, "%s", to2c[type_op]);
        }
    }
    return buffer;
}

void Type::initTO2c(void)
{
    to2c[TO_CHAR] = "char";
    to2c[TO_INT] = "int";
    to2c[TO_LONG] = "long";
    to2c[TO_SHORT] = "short";
    to2c[TO_FLOAT] = "float";
    to2c[TO_ARRAY] = "array";
    to2c[TO_STRUCT] = "struct";
    to2c[TO_ENUM] = "enum";
    to2c[TO_POINTER] = "pointer";
    to2c[TO_FUNCTION] = "function";
    to2c[TO_CONST] = "const";
    to2c[TO_VOLATILE] = "volatile";
    to2c[TO_SIGNED] = "signed";
    to2c[TO_UNSIGNED] = "unsigned";
}

const char *Type::getTYPEName(void)
{
    print();
    return name;
}

const TypeException &Type::checkFunction(const Type &ty, TypeException &te)
{
    assert(ty.type_op == TO_FUNCTION);

    if (ty.base_type->type_op == TO_FUNCTION)
        te.setError(typeError[TE_FUNC_RET_FUNC]);
    if (ty.base_type->type_op == TO_ARRAY)
        te.setError(typeError[TE_FUNC_RET_ARR]);
    if (ty.u.f.paraList == NULL)
        te.setError("internal error.");
    else {
        Type::checkPara(*ty.u.f.paraList, te);
        if (!te.no_exception()) {
            te.setError(te.toString());
        }
    }
    return te;
}

const TypeException &Type::checkPara(const Type &para, TypeException &te)
{
    if (para.u.f.paraList == NULL)
        return te;
    else {
        if (para.u.f.paraList->type_op != TO_FUNCPARA)
            te.setError("internal error.");
        else if (para.u.f.paraList->base_type->type_op == TO_FUNCTION) {
            te.setError(typeError[TE_FUNC_NEED_FUNC]);
        }
        Type::checkPara(*para.u.f.paraList, te);
    }
    return te;
}

const TypeException &Type::check(const Type &ty)
{
    static TypeException te;
    Type::checkType(ty, te);
    return te;
}

const TypeException &Type::checkArray(const Type &ty, TypeException &te)
{
    assert(ty.type_op == TO_ARRAY);
    if (ty.base_type->type_op == TO_FUNCTION)
        te.setError(typeError[TE_ARR_CONT_FUNC]);
    else
        te = Type::checkType(*ty.base_type, te);
    return te;
}

const Type &Type::binaryOp(const Type &left, const Type &right)
{
    const Type *leftTY = &left;
    while (left.getTYOP() == TO_CONST || left.getTYOP() == TO_VOLATILE) {
        leftTY = &leftTY->getBaseType();
    }

    const Type *rightTY = &right;
    while (left.getTYOP() == TO_CONST || left.getTYOP() == TO_VOLATILE)

    {
        leftTY = &leftTY->getBaseType();
    }

    return *NS_BASE_TYPE::intType;
}

namespace NS_BASE_TYPE
{
    Type *charType;
    Type *ucharType;
    Type *scharType;
    Type *shortType;
    Type *ushortType;
    Type *intType;
    Type *uintType;
    Type *longType;
    Type *ulongType;
    Type *floatType;
    Type *doubleType;
    Type *voidType;
    Type *pvType;

    void InitBaseTypes(void)
    {
        try {
            charType = new Type(TO_CHAR, NULL, 1);
            ucharType = new Type(TO_UNSIGNED, charType, 1);
            scharType = new Type(TO_SIGNED, charType, 1);

            shortType = new Type(TO_SHORT);
            ushortType = new Type(TO_CHAR);

            intType = new Type(TO_INT);
            uintType = new Type(TO_UNSIGNED, intType);

            longType = new Type(TO_LONG);
            ulongType = new Type(TO_UNSIGNED, longType);

            floatType = new Type(TO_FLOAT);
            doubleType = new Type(TO_DOUBLE, NULL, 8);

            voidType = new Type(TO_VOID);
            pvType = new Type(TO_POINTER, voidType);
        } catch (std::bad_alloc e) {
            fatalError("new BASE_TYPES error: no more memory.");
        }
    }

    void DeInitBaseTypes(void)
    {
        delete charType;
        delete ucharType;
        delete scharType;
        delete shortType;
        delete ushortType;
        delete intType;
        delete uintType;
        delete longType;
        delete ulongType;
        delete floatType;
        delete doubleType;
        delete voidType;
        delete pvType;
    }
};  // namespace NS_BASE_TYPE
/*
const char* TypeException::toString( void ) const
{
	return ex;
}
*/

TypeException::~TypeException() {}

void TypeException::setException(int exT, const char *exS)
{
    Exception::setException(exT, exS);
}