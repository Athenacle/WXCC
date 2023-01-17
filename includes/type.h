/* type header
* WangXiaochun
* zjjhwxc(at)gmail.com
* Mar 6, 2013
*
*/

#ifndef TYPE_H
#define TYPE_H

#include <map>

#include "exception.h"
#include "semantic.h"
#include "symbol.h"

namespace type_operator
{
    /* enum type_operator
	* 0x0100
	*  [0 0 0 0 0 0 0 0]  [0 0 0 0 0 0 0 0]     [0 0 0 0 0 0 0 0] [0 0 0 0 0 0 0 0]
	*   0: BASIC_TYPE      0: AGGREGATE_TYPE     0: SIGNED
	*   1: NOT_BASIC TYPE  1: NOT_AGGREGATE_TYPE 1: UNSIGNED
	*   2: POINTER_TYPE                          2: NONE_SIG
	*   3: FUNCTION_TYPE
	*   4: SPECIFIER_TYPE
	*/
    enum TYPE_OPERATOR {
        TO_VOID = 0x01000000,
        /* BASIC_TYPE		*/
        TO_CHAR = 0x01000001,
        TO_INT = 0x01000012,
        TO_LONG = 0x01000013,
        TO_SHORT = 0x01000014,
        TO_FLOAT = 0x01000015,
        TO_DOUBLE = 0x01000016,
        /* AGGREGATE_TYPE	*/
        TO_STRUCT = 0x01000101,
        TO_UNION = 0x01000102,
        TO_ARRAY = 0x01000103,
        TO_ENUM = 0x01000104,
        /* POINTER_TYPE		*/
        TO_POINTER = 0x01000200,
        /* FUNCTION_TYPE	*/
        TO_FUNCTION = 0x01000300,
        /* SPECIFIER_TYPE	*/
        TO_CONST = 0x01004001,
        TO_VOLATILE = 0x01004002,

        TO_SIGNED = 0x01005001,
        TO_UNSIGNED = 0x01005002,

        TO_FUNCPARA = 0x01001111
    };

    const unsigned long BASE_TYPE_MASK = 0x01000010u;
    const unsigned long SIGNED_MASK = 0x01005000u;
    const unsigned long NO_LIMIT = 0x7fffffffu;
    /* because the sizeof operator cannot be operated on a function.
	*	i.e. the size of a function cannot be calculated.
	*	so, set the size field of the function type to NO_LIMIT.
	* Just a magic number, nothing else.
	*/
};  // namespace type_operator

using namespace type_operator;


class TypeException : public Exception
{
public:
    TypeException(int tp = 0, const char *st = "no exception.") : Exception(tp, st) {}


    void setException(int, const char *);
    virtual ~TypeException();
};


class Type
{
    friend class Function;

    TYPE_OPERATOR type_op; /* enum type_operator	*/
    Type *base_type;       /* operand/return type	*/
    int align;             /* alignment size (byte)*/
    int size;              /* total size (byte)	*/
    Symbol *sym;

    const char *name;

    static std::map<TYPE_OPERATOR, const char *> to2c;

    union {
        struct function_para {
            Type *paraList;
            /* the function type. parameter sequential list, which is
			* terminated by a ((struct type *)NULL) pointer.
			*
			* when the function has a variadic parameter, the end of the
			* list should contain the TO_VOID, then the NULL.
			*/
        } f;
        struct array_nElement {
            int nElements;
        } a;
    } u;


    void print(void);
    char *print(char *);

    static const TypeException &checkPointer(const Type &, TypeException &);

    static const TypeException &checkPara(const Type &, TypeException &);

    static const TypeException &checkFunction(const Type &, TypeException &);

    static const TypeException &checkArray(const Type &, TypeException &);

    static const TypeException &checkType(const Type &ty, TypeException &te);

    static bool matchBaseType(const Type &ty)
    {
        return (ty.type_op & BASE_TYPE_MASK) == BASE_TYPE_MASK || ty.type_op == TO_CHAR;
    }

    static bool matchSigned(const Type &ty)
    {
        return (ty.type_op & SIGNED_MASK) == SIGNED_MASK;
    }

public:
    explicit Type(type_operator::TYPE_OPERATOR = type_operator::TO_VOID,
                  Type * = nullptr,
                  int = 0,
                  Type * = nullptr,
                  int = 4,
                  int = 4,
                  void * = nullptr);

    int getSize(void) const
    {
        return size;
    }

    Type &setBaseType(Type *ty)
    {
        this->base_type = ty;
        return *ty;
    }

    type_operator::TYPE_OPERATOR getTYOP(void) const
    {
        return this->type_op;
    }


    static void initTO2c(void);

    const char *getTYPEName(void);

    void setSym(Symbol *sy)
    {
        if (sym == nullptr)
            sym = new Symbol(*sy);
        else
            assert(0);
    }

    static void setAsPara(Type &ty)
    {
        assert(ty.type_op != TO_FUNCPARA);
        ty.type_op = TO_FUNCPARA;
    }

    void nextPara(Type *ty)
    {
        assert(ty->type_op == TO_FUNCPARA);
        this->u.f.paraList = ty;
    }

    void setReturnType(Type *ty)
    {
        assert(this->type_op == TO_FUNCTION || ty->type_op == TO_FUNCTION);
        this->base_type = ty;
    }


    static const TypeException &check(const Type &);

    operator bool()
    {
        return Type::check(*this).no_exception();
    }

    const Type &getParaList(void) const
    {
        assert(this->type_op == TO_FUNCPARA || this->type_op == TO_FUNCTION);
        return *this->u.f.paraList;
    }

    const Type &getBaseType(void) const
    {
        return *this->base_type;
    }

    static const Type &binaryOp(const Type &, const Type &);

    int getArrayElemNum(void) const
    {
        assert(this->getTYOP() == TO_ARRAY);
        return this->u.a.nElements;
    }
};

/* EXAMPLES for types.
*
* 1.int a; the type struct in the symbol of 'a' should be
*	{TO_INT, NULL, 4, 4, p_a, (NULL)}
*
* 2.int array[10];
*	{ARRAY, {TO_INT, NULL, 4, 4, NULL, NULL}, 4, 40, p_arr, (10)}
*
*
*   int array[10][10];
*	{ARRAY,
*		{ARRAY,
*			{TO_INT, NULL, 4, 4, NULL, NULL},
*		4, 40, NULL, (10)},
*	4, 400, p_arr, (10)}
*
*   extern int array[];	--  incompleted type
*      {ARRAY, {TO_INT, NULL, 4, 4, NULL, NULL}, 4, 0, p_arr, (0)}
*
*
* 3.const volatile int * p;
*	{TO_POINTER
*	        {TO_CONST,
*	              {TO_VOLATILE,
*	                      {TO_INT, NULL, 4, 4, NULL, (NULL)},
*	              4, 4, NULL, (NULL)}
*	      4 , 4, NULL, (NULL)}
*	 4, 4, p_p, (NULL)}
*
*   const int* volatile p;
*	{TO_VOLATILE
*	        {TO_POINTER,
*	              {TO_CONST,
*	                      {TO_INT, NULL, 4, 4, NULL, (NULL)},
*	              4, 4, NULL, (NULL)}
*	      4 , 4, NULL, (NULL)}
*	 4, 4, p_p, (NULL)}
*   void *p;
*	{TO_POINTER,
*		{TO_VOID, NULL, 0, 0, NULL, (NULL)},
*	4, 4, p_p, (NULL)}
*
*
* 5.int fun(void);
*	{TO_FUNCTION,
*		{TO_INT, NULL, 4, 4, NULL, (NULL)}
4, NO_LIMITED, p_fun, ({TO_VOID, NULL, 0, 0, NULL, NULL} -> NULL)}.
*
*   void fun(int a, double b);
*	{TO_FUNCTION,
*		{TO_VOID, NULL, 0, 0, NULL, (NULL)},
*	4, NO_LIMITED, p_fun, ({TO_INT,NULL, 4, 4, NULL, NULL} ->
*		{TO_DOUBLE, NULL, 8, 8, NULL, NULL} -> NULL) }
*
*   int printf(const char * format, ...)
*	{TO_FUNCTION,
*		{TO_INT, NULL, 4, 4, NULL, (NULL)},
*			4, NO_LIMITED, p_printf,
*			({TO_POINTER,
*				{TO_CONST,
*					{TO_CHAR, NULL, 4, 1, NULL, (NULL)},
*						4, 1, NULL, (NULL)},
*				4, 4, NULL, NULL} ->
*				{TO_VOID, NULL, 0, 0, NULL, NULL} -> NULL)}
*
*/


#define SEM_ERR_TOO_TYPE 1

namespace NS_BASE_TYPE
{
    extern Type *charType;
    extern Type *ucharType;
    extern Type *scharType;

    extern Type *shortType;
    extern Type *ushortType;

    extern Type *intType;
    extern Type *uintType;

    extern Type *longType;
    extern Type *ulongType;

    extern Type *floatType;
    extern Type *doubleType;

    extern Type *voidType;
    extern Type *pvType;

    void InitBaseTypes(void);
    void DeInitBaseTypes(void);
};  // namespace NS_BASE_TYPE
#endif