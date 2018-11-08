/*******************************************************************************
 * Name            : ast.h
 * Project         : fcal
 * Module          : ast
 * Description     : Header file for AST
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Author : Eric Van Wyk
 * Modifications by: Aadil Naumaan and Sifora Tek-Lab
 ******************************************************************************/

#ifndef PROJECT_INCLUDE_AST_H_
#define PROJECT_INCLUDE_AST_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <iostream>
#include <string>
#include "include/scanner.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace fcal {
namespace ast {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/

/*!  This is the root node class of the syntax tree.
     Stmts, Stmt, Decl, and Expr classes are derived from this class.
     All classes derived from node contain UnParse function, which
     converts the AST back to DSL
*/
class Node {
 public:
  virtual std::string UnParse(void) { return " This should be pure virtual "; }
  virtual std::string CppCode(void) { return " This should be pure virtual"; }
  // virtual std::string CppCode(void) = 0;
  virtual ~Node(void) {}
};

/*!
  These are the instantiations of the abstract classes.
  All concrete classes will inherit from one of these classes.
*/
class Stmts : public Node {};
class Stmt : public Node {};
class Decl : public Node {};
class Expr : public Node {};

/*!
    This class represents a variable name within a production.
    This is a concrete class
*/
class VarName {
 public:
  explicit VarName(std::string lexeme) : lexeme_(lexeme) {}
  std::string UnParse();
  std::string CppCode();

 private:
  VarName() : lexeme_((std::string) "") {}
  VarName(const VarName &) {}
  std::string lexeme_;
};

// Root or Program Node
/*!
    This class represents the program production, which has the form:
    Program ::= varName '(' ')' '{' Stmts '}'
    There are no classes which inherit from the root class.
*/
class Root : public Node {
 public:
  explicit Root(VarName *var_name, Stmts *stmts)
      : var_name_(var_name), stmts_(stmts) {}
  std::string UnParse();
  std::string CppCode();
  virtual ~Root();

 private:
  Root() : var_name_(NULL), stmts_(NULL) {}
  Root(const Root &) {}
  VarName *var_name_;
  Stmts *stmts_;
};

// Stmts
// -----------------------------------------------------------

/*!
    This class encompasses productions with the following form:
    Stmts ::= Stmt Stmts
*/
class StmtsSeq : public Stmts {
 public:
  StmtsSeq(Stmt *stmt, Stmts *stmts);
  std::string UnParse();
  std::string CppCode();

 private:
  StmtsSeq() : stmt_(NULL), stmts_(NULL) {}
  StmtsSeq(const StmtsSeq &) {}
  Stmt *stmt_;
  Stmts *stmts_;
};

class EmptyStmts : public Stmts {
 public:
  EmptyStmts() {}
  std::string UnParse();
  std::string CppCode();

 private:
  EmptyStmts(const EmptyStmts &) {}
};

// Stmt
// -----------------------------------------------------------
/*!
    This class encompasses productions with the following form:
     Stmt ::= Decl
*/
class DeclStmt : public Stmt {
 public:
  explicit DeclStmt(Decl *decl) : decl_(decl) {}
  std::string UnParse();
  std::string CppCode();

 private:
  DeclStmt() : decl_(NULL) {}
  DeclStmt(const DeclStmt &) {}
  Decl *decl_;
};

/*!
    This class encompsses a production with the following form:
     Stmt ::= '{' Stmts '}'
*/
class StmtStmts : public Stmt {
 public:
  explicit StmtStmts(Stmts *stmts) : stmts_(stmts) {}
  std::string UnParse();
  std::string CppCode();

 private:
  StmtStmts() : stmts_(NULL) {}
  StmtStmts(const StmtStmts &) {}
  Stmts *stmts_;
};

/*!
    This class encompasses any production with form:
    Stmt :== 'if' '(' Expr ')' Stmt
*/
class IfStmt : public Stmt {
 public:
  IfStmt(Expr *expr, Stmt *stmt);
  std::string UnParse();
  std::string CppCode();

 private:
  IfStmt() : expr_(NULL), stmt_(NULL) {}
  IfStmt(const IfStmt &) {}
  Expr *expr_;
  Stmt *stmt_;
};

/*!
    This class encompasses any production with this form:
    Stmt ::= 'if' '(' Expr ')' Stmt 'else' Stmt
*/
class IfElseStmt : public Stmt {
 public:
  IfElseStmt(Expr *expr, Stmt *stmt1, Stmt *stmt2);
  std::string UnParse();
  std::string CppCode();

 private:
  IfElseStmt() : expr_(NULL), stmt1_(NULL), stmt2_(NULL) {}
  IfElseStmt(const IfElseStmt &) {}
  Expr *expr_;
  Stmt *stmt1_;
  Stmt *stmt2_;
};

/*!
    This class encompasses any production with this form:
    Stmt ::= varName '=' Expr ';'
*/
class AssignStmt : public Stmt {
 public:
  AssignStmt(VarName *var_name, Expr *expr);
  std::string UnParse();
  std::string CppCode();

 private:
  AssignStmt() : var_name_(NULL), expr_(NULL) {}
  AssignStmt(const AssignStmt &) {}
  VarName *var_name_;
  Expr *expr_;
};

/*!
    This class encompasses productions with the following form:
    Stmt ::= varName '[' Expr ':' Expr ']' '=' Expr ';'
*/
class AssignMatrixStmt : public Stmt {
 public:
  AssignMatrixStmt(VarName *var_name, Expr *expr1, Expr *expr2, Expr *expr3);
  std::string UnParse();
  std::string CppCode();

 private:
  AssignMatrixStmt()
      : var_name_(NULL), expr1_(NULL), expr2_(NULL), expr3_(NULL) {}
  AssignMatrixStmt(const AssignMatrixStmt &) {}
  VarName *var_name_;
  Expr *expr1_;
  Expr *expr2_;
  Expr *expr3_;
};

/*!
    This class encompasses productions with the following form:
    Stmt ::= 'print' '(' Expr ')' ';'
*/
class PrintStmt : public Stmt {
 public:
  explicit PrintStmt(Expr *expr) : expr_(expr) {}
  std::string UnParse();
  ;
  std::string CppCode();

 private:
  PrintStmt() : expr_(NULL) {}
  PrintStmt(const PrintStmt &) {}
  Expr *expr_;
};

/*!
    This class encompasses productions with the following form:
    Stmt ::= 'repeat' '(' varName '=' Expr 'to' Expr ')' Stmt
*/
class RepeatStmt : public Stmt {
 public:
  RepeatStmt(VarName *var_name, Expr *expr1, Expr *expr2, Stmt *stmt);
  std::string UnParse();
  std::string CppCode();

 private:
  RepeatStmt() : var_name_(NULL), expr1_(NULL), expr2_(NULL), stmt_(NULL) {}
  RepeatStmt(const RepeatStmt &) {}
  VarName *var_name_;
  Expr *expr1_;
  Expr *expr2_;
  Stmt *stmt_;
};

/*!
    This class encompasses productions with the following form:
    Stmt ::= 'while' '(' Expr ')' Stmt
*/
class WhileStmt : public Stmt {
 public:
  WhileStmt(Expr *expr, Stmt *stmt);
  std::string UnParse();
  std::string CppCode();

 private:
  WhileStmt() : expr_(NULL), stmt_(NULL) {}
  WhileStmt(const WhileStmt &) {}
  Expr *expr_;
  Stmt *stmt_;
};

/*!
    This concrete class encompasses productions with the following form:
    Stmt ::= ';'
*/
class SemiStmt : public Stmt {
 public:
  SemiStmt();
  std::string UnParse();
  std::string CppCode();

 private:
  SemiStmt(const SemiStmt &) {}
};

// Decl
// -----------------------------------------------------------
/*!
    This class encompasses productions with the following form:
    Decl ::= 'int' varName ';'
*/
class IntDecl : public Decl {
 public:
  explicit IntDecl(VarName *var_name) : var_name_(var_name) {}
  std::string UnParse();
  std::string CppCode();

 private:
  IntDecl() : var_name_(NULL) {}
  IntDecl(const IntDecl &) {}
  VarName *var_name_;
};

/*!
    This class encompasses productions with the following form:
    Decl ::= 'float' varName ';'
*/
class FloatDecl : public Decl {
 public:
  explicit FloatDecl(VarName *var_name) : var_name_(var_name) {}
  std::string UnParse();
  std::string CppCode();

 private:
  FloatDecl() : var_name_(NULL) {}
  FloatDecl(const FloatDecl &) {}
  VarName *var_name_;
};

/*!
    This class encompasses productions with the following form:
    Decl ::= 'string' varName ';'
*/
class StringDecl : public Decl {
 public:
  explicit StringDecl(VarName *var_name) : var_name_(var_name) {}
  std::string UnParse();
  std::string CppCode();

 private:
  StringDecl() : var_name_(NULL) {}
  StringDecl(const StringDecl &) {}
  VarName *var_name_;
};

/*!
    This class encompasses productions with the following form:
    Decl ::= 'boolean' varName ';'
*/
class BooleanDecl : public Decl {
 public:
  explicit BooleanDecl(VarName *var_name) : var_name_(var_name) {}
  std::string UnParse();
  std::string CppCode();

 private:
  BooleanDecl() : var_name_(NULL) {}
  BooleanDecl(const BooleanDecl &) {}
  VarName *var_name_;
};

/*!
    This class encompasses productions with the following form:
    Decl ::= 'matrix' varName '=' Expr ';'
*/
class MatrixDecl : public Decl {
 public:
  MatrixDecl(VarName *var_name, Expr *expr);
  std::string UnParse();
  std::string CppCode();

 private:
  MatrixDecl() : var_name_(NULL), expr_(NULL) {}
  MatrixDecl(const MatrixDecl &) {}
  VarName *var_name_;
  Expr *expr_;
};

/*!
    This class encompasses productions with the following form:
    Decl ::= 'matrix' varName '[' Expr ':' Expr ']' varName ':' varName  '='
   Expr ';'
*/
class LongMatrixDecl : public Decl {
 public:
  LongMatrixDecl(VarName *var_name1, VarName *var_name2, VarName *var_name3,
                 Expr *expr1, Expr *expr2, Expr *expr3);
  std::string UnParse();
  std::string CppCode();

 private:
  LongMatrixDecl()
      : var_name1_(NULL),
        var_name2_(NULL),
        var_name3_(NULL),
        expr1_(NULL),
        expr2_(NULL),
        expr3_(NULL) {}
  LongMatrixDecl(const LongMatrixDecl &) {}
  VarName *var_name1_;
  VarName *var_name2_;
  VarName *var_name3_;
  Expr *expr1_;
  Expr *expr2_;
  Expr *expr3_;
};

// Expr
// -----------------------------------------------------------
/*!
    This class encompasses productions with the following forms:
    Expr ::= Expr '*' Expr
    Expr ::= Expr '/' Expr
    Expr ::= Expr '+' Expr
    Expr ::= Expr '-' Expr
    Expr ::= Expr '>' Expr
    Expr ::= Expr '>=' Expr
    Expr ::= Expr '<' Expr
    Expr ::= Expr '<=' Expr
    Expr ::= Expr '==' Expr
    Expr ::= Expr '!=' Expr
    Expr ::= Expr '&&' Expr
    Expr ::= Expr '||' Expr
*/
class BinaryOpExpr : public Expr {
 public:
  BinaryOpExpr(Expr *expr1, std::string op, Expr *expr2);
  std::string UnParse();
  std::string CppCode();

 private:
  BinaryOpExpr() : expr1_(NULL), operator_(NULL), expr2_(NULL) {}
  BinaryOpExpr(const BinaryOpExpr &) {}
  Expr *expr1_;
  std::string operator_;
  Expr *expr2_;
};

/*!
    This class encompasses productions with the following form:
    Expr ::= varName '[' Expr ':' Expr ']'
*/
class MatrixRefExpr : public Expr {
 public:
  MatrixRefExpr(VarName *var_name, Expr *expr1, Expr *expr2);
  std::string UnParse();
  std::string CppCode();

 private:
  MatrixRefExpr() : var_name_(NULL), expr1_(NULL), expr2_(NULL) {}
  MatrixRefExpr(const MatrixRefExpr &) {}
  VarName *var_name_;
  Expr *expr1_;
  Expr *expr2_;
};

/*!
    This class encompasses productions with the following form:
    Expr ::= 'True' | 'False'
*/
class BoolExpr : public Expr {
 public:
  explicit BoolExpr(bool boolean) : boolean_(boolean) {}
  std::string UnParse();
  // std::string CppCode();

 private:
  BoolExpr() : boolean_(NULL) {}
  BoolExpr(const BoolExpr &) {}
  bool boolean_;
};

// Variable name expression
/*!
    This class encompasses productions with the following form:
    Expr ::= varName
*/
class VarNameExpr : public Expr {
 public:
  explicit VarNameExpr(VarName *var_name) : var_name_(var_name) {}
  std::string UnParse();
  std::string CppCode();

 private:
  VarNameExpr() : var_name_(NULL) {}
  VarNameExpr(const VarNameExpr &) {}
  VarName *var_name_;
};

// Parentheses Expression
/*!
    This class encompasses productions with the following form:
    Expr ::= '(' Expr ')'
*/
class ParenExpr : public Expr {
 public:
  explicit ParenExpr(Expr *expr) : expr_(expr) {}
  std::string UnParse();
  std::string CppCode();

 private:
  ParenExpr() : expr_(NULL) {}
  ParenExpr(const ParenExpr &) {}
  Expr *expr_;
};

// Nested or function expression
/*!
    This class encompasses productions with the following form:
    Expr ::= varName '(' Expr ')'
*/
class NestedOrFunctionExpr : public Expr {
 public:
  NestedOrFunctionExpr(VarName *var_name, Expr *expr);
  std::string UnParse();
  std::string CppCode();

 private:
  NestedOrFunctionExpr() : var_name_(NULL), expr_(NULL) {}
  NestedOrFunctionExpr(const NestedOrFunctionExpr &) {}
  VarName *var_name_;
  Expr *expr_;
};

// Let Expression
/*!
    This class encompasses productions with the following form:
    Expr ::= 'let' Stmts 'in' Expr 'end'
*/
class LetExpr : public Expr {
 public:
  LetExpr(Stmts *stmts, Expr *expr);
  std::string UnParse();
  std::string CppCode();

 private:
  LetExpr() : stmts_(NULL), expr_(NULL) {}
  LetExpr(const LetExpr &) {}
  Stmts *stmts_;
  Expr *expr_;
};

// If Expression
/*!
    This class encompasses productions with the following form:
    Expr ::= 'if' Expr 'then' Expr 'else' Expr
*/
class IfExpr : public Expr {
 public:
  IfExpr(Expr *expr1, Expr *expr2, Expr *expr3);
  std::string UnParse();
  std::string CppCode();

 private:
  IfExpr() : expr1_(NULL), expr2_(NULL), expr3_(NULL) {}
  IfExpr(const IfExpr &) {}
  Expr *expr1_;
  Expr *expr2_;
  Expr *expr3_;
};

/*!
    This class encompasses productions with the following form:
    Expr ::= '!' Expr
*/
class NotExpr : public Expr {
 public:
  explicit NotExpr(Expr *expr) : expr_(expr) {}
  std::string UnParse();
  std::string CppCode();

 private:
  NotExpr() : expr_(NULL) {}
  NotExpr(const NotExpr &) {}
  Expr *expr_;
};

/*!
    This class encompasses productions with the following form:
    Expr ::= integerConst
*/
class IntConstExpr : public Expr {
 public:
  explicit IntConstExpr(std::string const_int) : const_int_(const_int) {}
  std::string UnParse();
  std::string CppCode();

 private:
  IntConstExpr() : const_int_(NULL) {}
  IntConstExpr(const IntConstExpr &) {}
  std::string const_int_;
};

// FloatConst expression
/*!
    This class encompasses production with the following form:
    Expr ::= floatConst
*/
class FloatConstExpr : public Expr {
 public:
  explicit FloatConstExpr(std::string const_float)
      : const_float_(const_float) {}
  std::string UnParse();
  std::string CppCode();

 private:
  FloatConstExpr();
  FloatConstExpr(const FloatConstExpr &) {}
  std::string const_float_;
};

// stringconst expression
/*!
    This class encompasses production of the following form:
    Expr ::= stringConst
*/
class StringConstExpr : public Expr {
 public:
  explicit StringConstExpr(std::string const_string)
      : string_const_(const_string) {}
  std::string UnParse();
  std::string CppCode();

 private:
  StringConstExpr() : string_const_(NULL) {}
  StringConstExpr(const StringConstExpr &) {}
  std::string string_const_;
};

} /* namespace ast */
} /* namespace fcal */

#endif  // PROJECT_INCLUDE_AST_H_
