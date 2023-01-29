#ifndef SEMANTIC_TYPE_H_
#define SEMANTIC_TYPE_H_

#include "system.h"

#include <list>
#include <tuple>
#include <vector>

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

        /* enum type_operator
         * 0x0100
         *       [0 0 0 0 0 0 0 0]  [0 0 0 0 0 0 0 0]     [0 0 0 0 0 0 0 0] [0 0 0 0 0 0 0 0]
         *        0: BASIC_TYPE      0: AGGREGATE_TYPE     0: SIGNED
         *        1: NOT_BASIC TYPE  1: NOT_AGGREGATE_TYPE 1: UNSIGNED
         *        2: POINTER_TYPE                          2: NONE_SIG
         *        3: FUNCTION_TYPE
         *        4: SPECIFIER_TYPE
         */

        TO_NONE     = 0x00000000,

        /* BASIC_TYPE        */
        TO_VOID     = 0x1100'0000,

        TO_CHAR     = 0x0100'0010,
        TO_INT      = 0x0100'0020,

        TO_LONG     = 0x0100'0100,
        TO_SHORT    = 0x0100'0200,

        TO_FLOAT    = 0x0100'1400,
        TO_DOUBLE   = 0x0100'1800,

        /* AGGREGATE_TYPE    */
        TO_STRUCT   = 0x0100'1100,
        TO_UNION    = 0x0100'1200,
        TO_ARRAY    = 0x0100'1400,
        TO_ENUM     = 0x0100'1800,

        /* POINTER_TYPE        */
        TO_POINTER  = 0x01000200,

        /* FUNCTION_TYPE    */
        TO_FUNCTION = 0x01000300,

        /* SPECIFIER_TYPE    */
        TO_CONST    = 0x0100'4001,
        TO_VOLATILE = 0x0100'4002,


        TO_SIGNED   = 0x0100'8001,
        TO_UNSIGNED = 0x0100'8002,

        TO_FUNCPARA = 0x01001111

    };
    class Type;

    class BaseType
    {
        friend class Type;

        TypeOperator operator_; /* enum type_operator     */

    public:
        ~BaseType();

        BaseType(TypeOperator op) : operator_(op) {}

        BaseType() : BaseType(TypeOperator::TO_NONE) {}
    };

    class Type
    {
    public:
        using QualifierContainer           = utils::BitsContainer<8>;

        constexpr static size_t Q_TYPEDEF  = 0;
        constexpr static size_t Q_EXTERN   = 1;
        constexpr static size_t Q_STATIC   = 2;
        constexpr static size_t Q_AUTO     = 3;
        constexpr static size_t Q_REGISTER = 4;
        constexpr static size_t Q_CONST    = 5;
        constexpr static size_t Q_VOLATILE = 6;
        constexpr static size_t Q_RESTRICT = 7;

    private:
        std::list<BaseType> base_;

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

        std::vector<std::tuple<int, std::string>> checkType() const;
    };

}  // namespace semantic
NAMESPACE_END

#endif
