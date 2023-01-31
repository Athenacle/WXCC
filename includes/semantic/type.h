#ifndef SEMANTIC_TYPE_H_
#define SEMANTIC_TYPE_H_

#include "system.h"

#include <list>
#include <tuple>
#include <variant>
#include <vector>

#include "lex/constants.h"
#include "utils/bit_container.h"

NAMESPACE_BEGIN


/* EXAMPLES for types.
*
* 1.int a; the type struct in the symbol of 'a' should be
*    {TO_INT, NULL, 4, 4, p_a, (NULL)}
*
* 2.int array[10];
*    {ARRAY, {TO_INT, NULL, 4, 4, NULL, NULL}, 4, 40, p_arr, (10)}
*
*
*   int array[10][10];
*    {ARRAY,
*        {ARRAY,
*            {TO_INT, NULL, 4, 4, NULL, NULL},
*        4, 40, NULL, (10)},
*    4, 400, p_arr, (10)}
*
*   extern int array[];    --  incompleted type
*      {ARRAY, {TO_INT, NULL, 4, 4, NULL, NULL}, 4, 0, p_arr, (0)}
*
*
* 3.const volatile int * p;
*    {TO_POINTER
*            {TO_CONST,
*                  {TO_VOLATILE,
*                          {TO_INT, NULL, 4, 4, NULL, (NULL)},
*                  4, 4, NULL, (NULL)}
*          4 , 4, NULL, (NULL)}
*     4, 4, p_p, (NULL)}
*
*   const int* volatile p;
*    {TO_VOLATILE
*            {TO_POINTER,
*                  {TO_CONST,
*                          {TO_INT, NULL, 4, 4, NULL, (NULL)},
*                  4, 4, NULL, (NULL)}
*          4 , 4, NULL, (NULL)}
*     4, 4, p_p, (NULL)}
*   void *p;
*    {TO_POINTER,
*        {TO_VOID, NULL, 0, 0, NULL, (NULL)},
*    4, 4, p_p, (NULL)}
*
*
* 5.int fun(void);
*    {TO_FUNCTION,
*        {TO_INT, NULL, 4, 4, NULL, (NULL)}
*    4, NO_LIMITED, p_fun, ({TO_VOID, NULL, 0, 0, NULL, NULL} -> NULL)}.
*
*   void fun(int a, double b);
*    {TO_FUNCTION,
*        {TO_VOID, NULL, 0, 0, NULL, (NULL)},
*    4, NO_LIMITED, p_fun, ({TO_INT,NULL, 4, 4, NULL, NULL} ->
*        {TO_DOUBLE, NULL, 8, 8, NULL, NULL} -> NULL) }
*
*   int printf(const char * format, ...)
*    {TO_FUNCTION,
*        {TO_INT, NULL, 4, 4, NULL, (NULL)},
*            4, NO_LIMITED, p_printf,
*            ({TO_POINTER,
*                {TO_CONST,
*                    {TO_CHAR, NULL, 4, 1, NULL, (NULL)},
*                        4, 1, NULL, (NULL)},
*                4, 4, NULL, NULL} ->
*                {TO_VOID, NULL, 0, 0, NULL, NULL} -> NULL)}
*
*/

namespace semantic
{
    enum class TypeOperator : uint32_t {
        TO_NONE     = 0x0000'0000,

        /* BASIC_TYPE        */
        TO_VOID     = 0x1100'0000,

        TO_CHAR     = 0x0100'0010,
        TO_INT      = 0x0100'0020,

        TO_SHORT    = 0x0100'0100,
        TO_LONG     = 0x0100'0200,
        TO_LONGLONG = 0x0100'0400,

        TO_FLOAT    = 0x0100'0400,
        TO_DOUBLE   = 0x0100'0800,

        TO_SIGNED   = 0x0a10'0000,
        TO_UNSIGNED = 0x0a20'0000,


        /* AGGREGATE_TYPE    */
        TO_STRUCT   = 0x0100'1100,
        TO_UNION    = 0x0100'1200,
        TO_ARRAY    = 0x0100'1400,
        TO_ENUM     = 0x0100'1800,


        /* POINTER_TYPE        */
        // TO_POINTER  = 0x01000200,

        /* FUNCTION_TYPE    */
        // TO_FUNCTION = 0x01000300,

        /* SPECIFIER_TYPE    */
        // TO_CONST    = 0x0100'4001,
        // TO_VOLATILE = 0x0100'4002,


        //TO_FUNCPARA = 0x01001111

    };
    class Type;

    extern std::vector<uint32_t> possibleTypeSpecifiers;

    using QualifierContainer           = utils::BitsContainer<8>;

    // storage class specifier
    constexpr static size_t Q_TYPEDEF  = 0;
    constexpr static size_t Q_EXTERN   = 1;
    constexpr static size_t Q_STATIC   = 2;
    constexpr static size_t Q_AUTO     = 3;
    constexpr static size_t Q_REGISTER = 4;

    // type qualifier
    constexpr static size_t Q_CONST    = 5;
    constexpr static size_t Q_VOLATILE = 6;
    constexpr static size_t Q_RESTRICT = 7;

    using WarningMessage               = std::tuple<int, std::string>;

    class TypeConversion
    {
        // refer to https://llvm.org/docs/LangRef.html#conversion-operations
        enum class Conversion {
            // TRUNC:  TRUNCate.
            // <result> = trunc <ty> <value> to <ty2>
            // %X = trunc i32 257 to i8                        ; yields i8:1
            // %Y = trunc i32 123 to i1                        ; yields i1:true
            TRUCT,

            // ZEXT: Zero-filled EXTend.
            // <result> = zext <ty> <value> to <ty2>
            // %X = zext i32 257 to i64                         ; yields i64:257
            // %Y = zext i1 true to i32                         ; yields i32:1
            ZEXT,

            // SEXT: Signed-filled EXTend.
            // <result> = sext <ty> <value> to <ty2>             ; yields ty2
            // %X = sext i8  -1 to i16                           ; yields i16   :65535
            // %Y = sext i1 true to i32                          ; yields i32:-1
            SEXT,

            // FPTRUNC: Floating Point TRUNCate.
            // <result> = fptrunc <ty> <value> to <ty2>           ; yields ty2
            // %X = fptrunc double 16777217.0 to float            ; yields float:16777216.0
            // %Y = fptrunc double 1.0E+300 to half               ; yields half:+infinity
            FPTRUNC,

            // FPEXT: Floating Point EXTend.
            // <result> = fpext <ty> <value> to <ty2>             ; yields ty2
            // %X = fpext float 3.125 to double                   ; yields double:3.125000e+00
            // %Y = fpext double %X to fp128                      ; yields fp128:0xL00000000000000004000900000000000
            FPEXT,

            // FPTOUI: FloatingPoint TO Unsigned Integer
            // <result> = fptoui <ty> <value> to <ty2>              ; yields ty2
            // %X = fptoui double 123.0 to i32                      ; yields i32:123
            // %Y = fptoui float 1.0E+300 to i1                     ; yields undefined:1
            // %Z = fptoui float 1.04E+17 to i8                     ; yields undefined:1
            FPTOUI,

            // FPTOSI: FloatingPoint TO Signed Integer.
            // <result> = fptosi <ty> <value> to <ty2>              ; yields ty2
            // %X = fptosi double -123.0 to i32                     ; yields i32:-123
            // %Y = fptosi float 1.0E-247 to i1                     ; yields undefined:1
            // %Z = fptosi float 1.04E+17 to i8                     ; yields undefined:1
            FPTOSI,

            // UITOFP: Unsigned Integer TO FloatingPoint
            // <result> = uitofp <ty> <value> to <ty2>              ; yields ty2
            // %X = uitofp i32 257 to float                         ; yields float:257.0
            // %Y = uitofp i8 -1 to double                          ; yields double:255.0
            UITOFP,

            // SITOFP: Signed Integer TO FloatingPoint
            // <result> = sitofp <ty> <value> to <ty2>              ; yields ty2
            // %X = sitofp i32 257 to float                         ; yields float:257.0
            // %Y = sitofp i8 -1 to double                          ; yields double:-1.0
            SITOFP,

            // PTRTOINT: PoinTER TO INTeger
            // <result> = ptrtoint <ty> <value> to <ty2>             ; yields ty2
            // %X = ptrtoint ptr %P to i8                            ; yields truncation on 32-bit architecture
            // %Y = ptrtoint ptr %P to i64                           ; yields zero extension on 32-bit architecture
            PTRTOINT,

            // INTTOPTR: INTeger TO PoinTER
            // <result> = inttoptr <ty> <value> to <ty2>...          ; yields ty2
            // X = inttoptr i32 255 to ptr                           ; yields zero extension on 64-bit architecture
            // %Y = inttoptr i32 255 to ptr                          ; yields no-op on 32-bit architecture
            // %Z = inttoptr i64 0 to ptr                            ; yields truncation on 32-bit architecture
            INTTOPTR,

            // BITCAST: BIT CAST
            // <result> = bitcast <ty> <value> to <ty2>             ; yields ty2
            // %X = bitcast i8 255 to i8          ; yields i8 :-1
            // %Y = bitcast i32* %x to i16*       ; yields i16*:%x
            BITCAST,

            // ADDRSPACECAST: ADDRess SPACE CAST
            // <result> = addrspacecast <pty> <ptrval> to <pty2>       ; yields pty2
            // %X = addrspacecast ptr %x to ptr addrspace(1)
            // %Y = addrspacecast ptr addrspace(1) %y to ptr addrspace(2)
            // %Z = addrspacecast <4 x ptr> %z to <4 x ptr addrspace(3)>
            ADDRSPACECAST
        };
    };

    struct BaseType {
        BaseType()                                  = default;
        virtual ~BaseType()                         = default;

        // print some type to string
        virtual std::string print() const           = 0;

        // compare two types are same
        virtual bool compare(const BaseType&) const = 0;

        static bool compare(const BaseType& first, const BaseType& second)
        {
            return first == second;
        }

        bool operator==(const BaseType& other) const
        {
            return this->compare(other);
        }


        struct TypeAssignmentCheckReturnType {
            bool can;
            std::variant<WarningMessage, TypeConversion> msg;
        };

        // check whether 'this' type can be assign from 'right' type
        virtual TypeAssignmentCheckReturnType assign(const BaseType& right) const = 0;


        static TypeAssignmentCheckReturnType assign(const BaseType& left, const BaseType& right)
        {
            return left.assign(right);
        }
    };

    class Type : public BaseType
    {
    public:
        bool compare(const BaseType&) const override;

        TypeAssignmentCheckReturnType assign(const BaseType& right) const override;

        std::string print() const override;

    private:
        std::list<TypeOperator> base_;

        QualifierContainer bits_;

    public:
        Type();

        ~Type();

        bool empty() const
        {
            return base_.empty();
        }

        void addBaseType(TypeOperator op);

        const auto& qualifierContainer() const
        {
            return bits_;
        }

        auto& qualifierContainer()
        {
            return bits_;
        }

        lex::constants::KEYWORD findQualifier() const;


        std::vector<std::tuple<int, std::string>> checkType() const;

        // virtual std::string print() const;
    };

    class FunctionType : public BaseType
    {
        Type* returnType_;
        std::vector<std::tuple<Type*, std::string>> param;
        bool variadic;

    public:
        FunctionType() = default;

        virtual ~FunctionType();
    };

    class ArrayType : public BaseType
    {
        Type* itemType_;
        int size_;

    public:
        ArrayType() = default;
        virtual ~ArrayType();
    };

    class PointerType : public BaseType
    {
        QualifierContainer typeQualifier_;
        Type* to_;

    public:
        PointerType() = default;
        virtual ~PointerType();
    };

    class StructUnionType : public BaseType
    {
    };

    class EnumerationType : public BaseType
    {
    };

}  // namespace semantic
NAMESPACE_END

#endif
