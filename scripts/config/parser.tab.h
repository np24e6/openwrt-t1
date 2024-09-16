/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_HELPTEXT = 258,              /* T_HELPTEXT  */
    T_WORD = 259,                  /* T_WORD  */
    T_WORD_QUOTE = 260,            /* T_WORD_QUOTE  */
    T_ALLNOCONFIG_Y = 261,         /* T_ALLNOCONFIG_Y  */
    T_BOOL = 262,                  /* T_BOOL  */
    T_CHOICE = 263,                /* T_CHOICE  */
    T_CLOSE_PAREN = 264,           /* T_CLOSE_PAREN  */
    T_COLON_EQUAL = 265,           /* T_COLON_EQUAL  */
    T_COMMENT = 266,               /* T_COMMENT  */
    T_CONFIG = 267,                /* T_CONFIG  */
    T_DEFAULT = 268,               /* T_DEFAULT  */
    T_DEFCONFIG_LIST = 269,        /* T_DEFCONFIG_LIST  */
    T_DEF_BOOL = 270,              /* T_DEF_BOOL  */
    T_DEF_TRISTATE = 271,          /* T_DEF_TRISTATE  */
    T_DEPENDS = 272,               /* T_DEPENDS  */
    T_ENDCHOICE = 273,             /* T_ENDCHOICE  */
    T_ENDIF = 274,                 /* T_ENDIF  */
    T_ENDMENU = 275,               /* T_ENDMENU  */
    T_HELP = 276,                  /* T_HELP  */
    T_DETAIL = 277,                /* T_DETAIL  */
    T_HEX = 278,                   /* T_HEX  */
    T_IF = 279,                    /* T_IF  */
    T_IMPLY = 280,                 /* T_IMPLY  */
    T_INT = 281,                   /* T_INT  */
    T_MAINMENU = 282,              /* T_MAINMENU  */
    T_MENU = 283,                  /* T_MENU  */
    T_MENUCONFIG = 284,            /* T_MENUCONFIG  */
    T_MODULES = 285,               /* T_MODULES  */
    T_ON = 286,                    /* T_ON  */
    T_OPEN_PAREN = 287,            /* T_OPEN_PAREN  */
    T_OPTION = 288,                /* T_OPTION  */
    T_OPTIONAL = 289,              /* T_OPTIONAL  */
    T_PLUS_EQUAL = 290,            /* T_PLUS_EQUAL  */
    T_PROMPT = 291,                /* T_PROMPT  */
    T_RANGE = 292,                 /* T_RANGE  */
    T_RESET = 293,                 /* T_RESET  */
    T_MAINTAINER = 294,            /* T_MAINTAINER  */
    T_SELECT = 295,                /* T_SELECT  */
    T_SOURCE = 296,                /* T_SOURCE  */
    T_STRING = 297,                /* T_STRING  */
    T_TRISTATE = 298,              /* T_TRISTATE  */
    T_VISIBLE = 299,               /* T_VISIBLE  */
    T_EOL = 300,                   /* T_EOL  */
    T_ASSIGN_VAL = 301,            /* T_ASSIGN_VAL  */
    T_OR = 302,                    /* T_OR  */
    T_AND = 303,                   /* T_AND  */
    T_EQUAL = 304,                 /* T_EQUAL  */
    T_UNEQUAL = 305,               /* T_UNEQUAL  */
    T_LESS = 306,                  /* T_LESS  */
    T_LESS_EQUAL = 307,            /* T_LESS_EQUAL  */
    T_GREATER = 308,               /* T_GREATER  */
    T_GREATER_EQUAL = 309,         /* T_GREATER_EQUAL  */
    T_NOT = 310                    /* T_NOT  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

	char *string;
	struct symbol *symbol;
	struct expr *expr;
	struct menu *menu;
	enum symbol_type type;
	enum variable_flavor flavor;


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
