%require "3.2"
%language "c++"
%define api.value.type variant
%define api.token.constructor
%defines
%define api.namespace {test}
%define api.parser.class {FlexBison_Parser}

%locations


%code requires{
namespace test{
      class FlexBison_Lexer;
      class FlexBison_Parser;
}
}

%code top{

// NOLINTBEGIN
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined GCC_COMPILABLE
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#include "flex_bison/misc/flex_bison_lexer.h"

namespace test{
  FlexBison_Parser::symbol_type
             yylex(FlexBison_Lexer&lval);
}
}

%lex-param {test::FlexBison_Lexer &scanner}
%parse-param {test::FlexBison_Lexer &scanner}
%define parse.trace
%define parse.error verbose
%define parse.assert true


%token EoF 0 "end of file"

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF

%token INC_OP               "++"
%token DEC_OP               "--"
%token LEFT_OP              "<<"
%token RIGHT_OP             ">>"
%token LE_OP                "<="
%token GE_OP                ">="
%token EQ_OP                "=="
%token NE_OP                "!="

%token PTR_OP               "->"

%token AND_OP               "&&"
%token OR_OP                "||"
%token MUL_ASSIGN           "*="
%token DIV_ASSIGN           "/="
%token MOD_ASSIGN           "%="
%token ADD_ASSIGN           "+="
%token SUB_ASSIGN           "-="
%token LEFT_ASSIGN          "<<="
%token RIGHT_ASSIGN         ">>="
%token AND_ASSIGN           "&="
%token XOR_ASSIGN           "^="
%token OR_ASSIGN            "|="

%token SEMICOLON            ";"
%token LEFTBRACE            "{"
%token RIGHTBRACE           "}"
%token LEFTSQBRAC           "["
%token RIGHTSQBRAC          "]"
%token LEFTBRACK            "("
%token RIGHTBRACK           ")"
%token PERIOD               "."
%token COMMA                ","
%token ASSIGN               "="
%token COLON                ":"
%token BITAND               "&"
%token LOGNOT               "!"
%token BITNOT               "~"
%token MINUS                "-"
%token PLUS                 "+"
%token MULT                 "*"
%token DIV                  "/"
%token MOD                  "%"
%token LT                   "<"
%token GT                   ">"
%token BITXOR               "^"
%token BITOR                "|"
%token QUESTION             "?"

%token TYPEDEF              "typedef"
%token EXTERN               "extern"
%token STATIC               "static"
%token AUTO                 "auto"
%token REGISTER             "register"
%token INLINE               "inline"
%token RESTRICT             "restrict"
%token CHAR                 "char"
%token SHORT                "short"
%token INT                  "int"
%token LONG                 "long"
%token VOID                 "void"
%token SIGNED               "signed"
%token UNSIGNED             "unsigned"
%token FLOAT                "float"
%token DOUBLE               "double"
%token CONST                "const"
%token VOLATILE             "volatile"
%token STRUCT               "struct"
%token UNION                "union"
%token ENUM                 "enum"
%token ELLIPSIS             "..."

%token CASE                 "case"
%token DEFAULT              "default"
%token IF                   "if"
%token ELSE                 "else"
%token SWITCH               "switch"
%token WHILE                "while"
%token DO                   "do"
%token FOR                  "for"
%token GOTO                 "goto"
%token CONTINUE             "continue"
%token BREAK                "break"
%token RETURN               "return"

%start translation_unit

%nonassoc ELSE

%%

primary_expression
      : IDENTIFIER
      | CONSTANT
      | STRING_LITERAL
      | LEFTBRACK expression RIGHTBRACE
      ;

postfix_expression
      : primary_expression
      | postfix_expression LEFTSQBRAC expression RIGHTSQBRAC
      | postfix_expression LEFTBRACE RIGHTBRACE
      | postfix_expression LEFTBRACE argument_expression_list RIGHTBRACE
      | postfix_expression PERIOD IDENTIFIER
      | postfix_expression PTR_OP IDENTIFIER
      | postfix_expression INC_OP
      | postfix_expression DEC_OP
      | LEFTBRACK type_name RIGHTBRACK LEFTBRACE initializer_list RIGHTBRACE
      | LEFTBRACK type_name RIGHTBRACK LEFTBRACE initializer_list COMMA RIGHTBRACE
      ;

argument_expression_list
      : assignment_expression
      | argument_expression_list COMMA assignment_expression
      ;

unary_expression
      : postfix_expression
      | INC_OP unary_expression
      | DEC_OP unary_expression
      | unary_operator cast_expression
      | SIZEOF unary_expression
      | SIZEOF LEFTBRACK type_name RIGHTBRACK
      ;

unary_operator
      : BITAND
      | MULT
      | PLUS
      | MINUS
      | BITNOT
      | LOGNOT
      ;

cast_expression
      : unary_expression
      | LEFTBRACK type_name RIGHTBRACK cast_expression
      ;

multiplicative_expression
      : cast_expression
      | multiplicative_expression MULT cast_expression
      | multiplicative_expression DIV cast_expression
      | multiplicative_expression MOD cast_expression
      ;

additive_expression
      : multiplicative_expression
      | additive_expression PLUS multiplicative_expression
      | additive_expression MINUS multiplicative_expression
      ;

shift_expression
      : additive_expression
      | shift_expression LEFT_OP additive_expression
      | shift_expression RIGHT_OP additive_expression
      ;

relational_expression
      : shift_expression
      | relational_expression LT shift_expression
      | relational_expression GT shift_expression
      | relational_expression LE_OP shift_expression
      | relational_expression GE_OP shift_expression
      ;

equality_expression
      : relational_expression
      | equality_expression EQ_OP relational_expression
      | equality_expression NE_OP relational_expression
      ;

and_expression
      : equality_expression
      | and_expression BITAND equality_expression
      ;

exclusive_or_expression
      : and_expression
      | exclusive_or_expression BITXOR and_expression
      ;

inclusive_or_expression
      : exclusive_or_expression
      | inclusive_or_expression BITOR exclusive_or_expression
      ;

logical_and_expression
      : inclusive_or_expression
      | logical_and_expression AND_OP inclusive_or_expression
      ;

logical_or_expression
      : logical_and_expression
      | logical_or_expression OR_OP logical_and_expression
      ;

conditional_expression
      : logical_or_expression
      | logical_or_expression QUESTION expression COLON conditional_expression
      ;

assignment_expression
      : conditional_expression
      | unary_expression assignment_operator assignment_expression
      ;

assignment_operator
      : ASSIGN
      | MUL_ASSIGN
      | DIV_ASSIGN
      | MOD_ASSIGN
      | ADD_ASSIGN
      | SUB_ASSIGN
      | LEFT_ASSIGN
      | RIGHT_ASSIGN
      | AND_ASSIGN
      | XOR_ASSIGN
      | OR_ASSIGN
      ;

expression
      : assignment_expression
      | expression COMMA assignment_expression
      ;

constant_expression
      : conditional_expression
      ;

declaration
      : declaration_specifiers SEMICOLON
      | declaration_specifiers init_declarator_list SEMICOLON
      ;

declaration_specifiers
      : storage_class_specifier
      | storage_class_specifier declaration_specifiers
      | type_specifier
      | type_specifier declaration_specifiers
      | type_qualifier
      | type_qualifier declaration_specifiers
      | function_specifier
      | function_specifier declaration_specifiers
      ;

init_declarator_list
      : init_declarator
      | init_declarator_list COMMA init_declarator
      ;

init_declarator
      : declarator
      | declarator ASSIGN initializer
      ;

storage_class_specifier
      : TYPEDEF
      | EXTERN
      | STATIC
      | AUTO
      | REGISTER
      ;

type_specifier
      : VOID
      | CHAR
      | SHORT
      | INT
      | LONG
      | FLOAT
      | DOUBLE
      | SIGNED
      | UNSIGNED
      | struct_or_union_specifier
      | enum_specifier
      ;

struct_or_union_specifier
      : struct_or_union IDENTIFIER LEFTBRACE struct_declaration_list RIGHTBRACE
      | struct_or_union LEFTBRACE struct_declaration_list RIGHTBRACE
      | struct_or_union IDENTIFIER
      ;

struct_or_union
      : STRUCT
      | UNION
      ;

struct_declaration_list
      : struct_declaration
      | struct_declaration_list struct_declaration
      ;

struct_declaration
      : specifier_qualifier_list struct_declarator_list SEMICOLON
      ;

specifier_qualifier_list
      : type_specifier specifier_qualifier_list
      | type_specifier
      | type_qualifier specifier_qualifier_list
      | type_qualifier
      ;

struct_declarator_list
      : struct_declarator
      | struct_declarator_list COMMA struct_declarator
      ;

struct_declarator
      : declarator
      | COLON constant_expression
      | declarator COLON constant_expression
      ;

enum_specifier
      : ENUM LEFTBRACE enumerator_list RIGHTBRACE
      | ENUM IDENTIFIER LEFTBRACE enumerator_list RIGHTBRACE
      | ENUM LEFTBRACE enumerator_list COMMA RIGHTBRACE
      | ENUM IDENTIFIER LEFTBRACE enumerator_list COMMA RIGHTBRACE
      | ENUM IDENTIFIER
      ;

enumerator_list
      : enumerator
      | enumerator_list COMMA enumerator
      ;

enumerator
      : IDENTIFIER
      | IDENTIFIER ASSIGN constant_expression
      ;

type_qualifier
      : CONST
      | RESTRICT
      | VOLATILE
      ;

function_specifier
      : INLINE
      ;

declarator
      : pointer direct_declarator
      | direct_declarator
      ;


direct_declarator
      : IDENTIFIER
      | LEFTBRACK declarator RIGHTBRACK
      | direct_declarator LEFTSQBRAC  type_qualifier_list assignment_expression RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  type_qualifier_list RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  assignment_expression RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  STATIC type_qualifier_list assignment_expression RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  type_qualifier_list STATIC assignment_expression RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  type_qualifier_list MULT RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  MULT RIGHTSQBRAC
      | direct_declarator LEFTSQBRAC  RIGHTSQBRAC
      | direct_declarator LEFTBRACK parameter_type_list RIGHTBRACK
      | direct_declarator LEFTBRACK identifier_list RIGHTBRACK
      | direct_declarator LEFTBRACK RIGHTBRACK
      ;

pointer
      : MULT
      | MULT type_qualifier_list
      | MULT pointer
      | MULT type_qualifier_list pointer
      ;

type_qualifier_list
      : type_qualifier
      | type_qualifier_list type_qualifier
      ;


parameter_type_list
      : parameter_list
      | parameter_list COMMA ELLIPSIS
      ;

parameter_list
      : parameter_declaration
      | parameter_list COMMA parameter_declaration
      ;

parameter_declaration
      : declaration_specifiers declarator
      | declaration_specifiers abstract_declarator
      | declaration_specifiers
      ;

identifier_list
      : IDENTIFIER
      | identifier_list COMMA IDENTIFIER
      ;

type_name
      : specifier_qualifier_list
      | specifier_qualifier_list abstract_declarator
      ;

abstract_declarator
      : pointer
      | direct_abstract_declarator
      | pointer direct_abstract_declarator
      ;

direct_abstract_declarator
      : LEFTBRACK abstract_declarator RIGHTBRACK
      | LEFTSQBRAC RIGHTSQBRAC
      | LEFTSQBRAC assignment_expression RIGHTSQBRAC
      | direct_abstract_declarator LEFTSQBRAC RIGHTSQBRAC
      | direct_abstract_declarator LEFTSQBRAC assignment_expression RIGHTSQBRAC
      | LEFTSQBRAC MULT RIGHTSQBRAC
      | direct_abstract_declarator   LEFTSQBRAC MULT RIGHTSQBRAC
      | LEFTBRACK RIGHTBRACK
      | LEFTBRACK parameter_type_list RIGHTBRACK
      | direct_abstract_declarator LEFTBRACK RIGHTBRACK
      | direct_abstract_declarator LEFTBRACK parameter_type_list RIGHTBRACK
      ;

initializer
      : assignment_expression
      | LEFTBRACE initializer_list RIGHTBRACE
      | LEFTBRACE initializer_list COMMA RIGHTBRACE
      ;

initializer_list
      : initializer
      | designation initializer
      | initializer_list COMMA initializer
      | initializer_list COMMA designation initializer
      ;

designation
      : designator_list ASSIGN
      ;

designator_list
      : designator
      | designator_list designator
      ;

designator
      : LEFTSQBRAC constant_expression RIGHTSQBRAC
      | PERIOD IDENTIFIER
      ;

statement
      : none_if_stmt
      | if_statement
      ;

none_if_stmt
      : labeled_statement
      | compound_statement
      | expression_statement
      | iteration_statement
      | jump_statement
      | switch_stmt
      ;

labeled_statement
      : IDENTIFIER COLON statement
      | CASE constant_expression COLON statement
      | DEFAULT COLON statement
      ;

compound_statement
      : LEFTBRACE RIGHTBRACE
      | LEFTBRACE block_item_list RIGHTBRACE
      ;

block_item_list
      : block_item
      | block_item_list block_item
      ;

block_item
      : declaration
      | statement
      ;

expression_statement
      : SEMICOLON
      | expression SEMICOLON
      ;
/*
selection_statement
      : IF LEFTBRACK expression RIGHTBRACK statement
      | IF LEFTBRACK expression RIGHTBRACK statement ELSE statement
      | SWITCH LEFTBRACK expression RIGHTBRACK statement
      ;
*/

switch_stmt
      : SWITCH LEFTBRACK expression RIGHTBRACK statement
      ;
/*
selection_statement
      : if_statement
      | 
      ;
*/
/*      : IF LEFTBRACK expression RIGHTBRACK statement %prec LOWER_THEN_ELSE
      | IF LEFTBRACK expression RIGHTBRACK statement ELSE statement
      ;*/
if_statement
      : if_branch none_if_stmt
      | if_branch else_branch
      ;

else_branch:
      ELSE statement
      ;

if_branch
      :  IF LEFTBRACK expression RIGHTBRACK statement


iteration_statement
      : WHILE LEFTBRACK expression RIGHTBRACK statement
      | DO statement WHILE LEFTBRACK expression RIGHTBRACK SEMICOLON
      | FOR LEFTBRACK expression_statement expression_statement RIGHTBRACK statement
      | FOR LEFTBRACK expression_statement expression_statement expression RIGHTBRACK statement
      | FOR LEFTBRACK declaration expression_statement RIGHTBRACK statement
      | FOR LEFTBRACK declaration expression_statement expression RIGHTBRACK statement
      ;

jump_statement
      : GOTO IDENTIFIER SEMICOLON
      | CONTINUE SEMICOLON
      | BREAK SEMICOLON
      | RETURN SEMICOLON
      | RETURN expression SEMICOLON
      ;

translation_unit
      : external_declaration
      | translation_unit external_declaration
      | EoF
      ;

external_declaration
      : function_definition
      | declaration
      ;

function_definition
      : declaration_specifiers declarator declaration_list compound_statement
      | declaration_specifiers declarator compound_statement
      ;

declaration_list
      : declaration
      | declaration_list declaration
      ;


%%

namespace test{

void FlexBison_Parser::error(const location &loc , const std::string &message) {

        std::cerr << "error" << loc << ": " << message << std::endl;
}

FlexBison_Parser::symbol_type yylex(FlexBison_Lexer&scanner){
      return scanner.next();
}

}

// NOLINTEND
