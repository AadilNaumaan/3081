/*******************************************************************************
 * Name            : ast.c
 * Project         : fcal
 * Module          : ast
 * Description     : Implementations for AST
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Author : Eric Van Wyk
 * Modifications by: Aadil Naumaan and Sifora Tek-Lab
 ******************************************************************************

/******************************************************************************
 * Includes
 ******************************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include "include/scanner.h"
#include "include/ast.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace fcal {
namespace ast {

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/

// Root
// ---------------------------------------------------------
/*!
    Unparse method for Root class. When Unparsed it has the following form:
    Program ::= varName '(' ')' '{' Stmts '}'
*/
std::string Root::UnParse() {
  return var_name_->UnParse() + " () {\n" + stmts_->UnParse() + "\n}\n";
}

Root::~Root() { std::cout << "... destructing Root ...." << std::endl; }

std::string Root::CppCode() {
  return "#include <iostream>\n" +
         std::string("#include \"include/Matrix.h\"\n") +
         "#include <math.h>\n" + "using namespace std; \n" +
         "int main () { \n" + stmts_->CppCode() + "\n}\n";
}
// VarName
// -----------------------------------------------------------
/*!
    Unparse method for VarName class. It simply returns the lexeme
*/
std::string VarName::UnParse() { return lexeme_; }

std::string VarName::CppCode() {
  if (lexeme_ == "matrix_read") return "matrix::matrix_read ";
  return lexeme_;
}
// Stmts
// -----------------------------------------------------------
StmtsSeq::StmtsSeq(Stmt *s, Stmts *ss) {
  stmt_ = s;
  stmts_ = ss;
}
/*!
    Unparse method for StmtStmts class. When Unparsed it has the following form:
    Program ::= varName '(' ')' '{' Stmts '}'
*/
std::string StmtsSeq::UnParse() { return stmt_->UnParse() + stmts_->UnParse(); }

std::string StmtsSeq::CppCode() { return stmt_->CppCode() + stmts_->CppCode(); }
/*!
    Unparse method for EmptyStmts. Returns nothing
*/
std::string EmptyStmts::UnParse() { return ""; }

std::string EmptyStmts::CppCode() {
  return "";  // there was a \n here
}

// Stmt
// -----------------------------------------------------------
/*!
    Unparse method for the DeclStmt class. When unparsed it has the form:
    Stmt ::= Decl
*/
std::string DeclStmt::UnParse() { return decl_->UnParse(); }

std::string DeclStmt::CppCode() { return decl_->CppCode(); }
/*!
    Unparse method for the StmtsStmts class. When unparsed it has the form:
    Stmt ::= '{' Stmts '}'
*/
std::string StmtStmts::UnParse() { return " { " + stmts_->UnParse() + " } "; }

std::string StmtStmts::CppCode() { return "{ \n" + stmts_->CppCode() + "} \n"; }

IfStmt::IfStmt(Expr *expr, Stmt *stmt) {
  expr_ = expr;
  stmt_ = stmt;
}

/*!
    Unparse method for the IfStmt class. When unparsed it has the form:
    Stmt :== 'if' '(' Expr ')' Stmt
*/
std::string IfStmt::UnParse() {
  return " if ( " + expr_->UnParse() + " ) \n" + stmt_->UnParse();
}

std::string IfStmt::CppCode() {
  return "if (" + expr_->CppCode() + ") " + stmt_->CppCode();
}

IfElseStmt::IfElseStmt(Expr *expr, Stmt *stmt1, Stmt *stmt2) {
  expr_ = expr;
  stmt1_ = stmt1;
  stmt2_ = stmt2;
}

/*!
    This is the Unparse method for the IfElseStmt class. When unparsed it has
   the form:
    Stmt ::= 'if' '(' Expr ')' Stmt 'else' Stmt
*/
std::string IfElseStmt::UnParse() {
  return " if ( " + expr_->UnParse() + " ) \n" + stmt1_->UnParse() +
         " else \n" + stmt2_->UnParse();
}

std::string IfElseStmt::CppCode() {
  return "( (" + expr_->CppCode() + ") ? (" + stmt1_->CppCode() + ") : " +
         stmt2_->CppCode() + " );";
}

AssignStmt::AssignStmt(VarName *var_name, Expr *expr) {
  var_name_ = var_name;
  expr_ = expr;
}
/*!
    This is the UnParse method for the AssignStmt class. When unparsed it has
   the form:
    Stmt ::= varName '=' Expr ';'
*/
std::string AssignStmt::UnParse() {
  return var_name_->UnParse() + " = " + expr_->UnParse() + "; \n";
}

std::string AssignStmt::CppCode() {
  return var_name_->CppCode() + " = " + expr_->CppCode() + " ; \n";
}

AssignMatrixStmt::AssignMatrixStmt(VarName *var_name, Expr *expr1, Expr *expr2,
                                   Expr *expr3) {
  var_name_ = var_name;
  expr1_ = expr1;
  expr2_ = expr2;
  expr3_ = expr3;
}
/*!
    This is the UnParse method for the AssignMatrixStmt class. When unparsed it
   has the form:
    Stmt ::= varName '[' Expr ':' Expr ']' '=' Expr ';'
*/
std::string AssignMatrixStmt::UnParse() {
  return var_name_->UnParse() + " [ " + expr1_->UnParse() + " : " +
         expr2_->UnParse() + " ] = " + expr3_->UnParse() + ";\n";
}

std::string AssignMatrixStmt::CppCode() {
  return "*(" + var_name_->CppCode() + ".access(" + expr1_->CppCode() + ", " +
         expr2_->CppCode() + ")) = " + expr3_->CppCode() + " ;";
}

/*!
    This is the UnParse method for the PrintStmt class. When unparsed it has the
   form:
    Stmt ::= 'print' '(' Expr ')' ';'
*/
std::string PrintStmt::UnParse() {
  return " print ( " + expr_->UnParse() + " ); \n";
}

std::string PrintStmt::CppCode() {
  return "cout << " + expr_->CppCode() + " ; \n";
}

RepeatStmt::RepeatStmt(VarName *var_name, Expr *expr1, Expr *expr2,
                       Stmt *stmt) {
  var_name_ = var_name;
  expr1_ = expr1;
  expr2_ = expr2;
  stmt_ = stmt;
}
/*!
    This is the UnParse method for the RepeatStmt class. When unparsed it has
   the form:
    Stmt ::= 'repeat' '(' varName '=' Expr 'to' Expr ')' Stmt
*/
std::string RepeatStmt::UnParse() {
  return " repeat ( " + var_name_->UnParse() + " = " + expr1_->UnParse() +
         " to " + expr2_->UnParse() + " ) " + stmt_->UnParse();
}

std::string RepeatStmt::CppCode() {
  return "for (" + var_name_->CppCode() + " = " + expr1_->CppCode() + "; " +
         var_name_->CppCode() + " <= " + expr2_->CppCode() + "; " +
         var_name_->CppCode() + " ++ )" + stmt_->CppCode();
}

WhileStmt::WhileStmt(Expr *expr, Stmt *stmt) {
  expr_ = expr;
  stmt_ = stmt;
}
/*!
    This is the UnParse method for the WhileStmt class. When unparsed it has the
   form:
    Stmt ::= 'while' '(' Expr ')' Stmt
*/
std::string WhileStmt::UnParse() {
  return " while ( " + expr_->UnParse() + " ) " + stmt_->UnParse();
}

std::string WhileStmt::CppCode() {
  return "while (" + expr_->CppCode() + " )" + stmt_->CppCode();
}

SemiStmt::SemiStmt() {}
/*!
    This is the UnParse method for the SemiStmt class. When unparsed it returns
   ';'
*/
std::string SemiStmt::UnParse() { return " ; \n"; }

std::string SemiStmt::CppCode() { return " ; \n"; }

// Decl
// -----------------------------------------------------------
/*!
    This is the UnParse method for the IntDecl class. When Unparsed it has the
   form:
    Decl ::= 'int' varName ';'
*/
std::string IntDecl::UnParse() {
  return " int " + var_name_->UnParse() + " ; \n";
}
std::string IntDecl::CppCode() {
  return "int " + var_name_->CppCode() + " ; \n";
}
/*!
    This is the Unparse method for the FloatDecl class. When unparsed it has the
   form:
    Decl ::= 'float' varName ';'
*/
std::string FloatDecl::UnParse() {
  return " float " + var_name_->UnParse() + " ; \n";
}

std::string FloatDecl::CppCode() {
  return "float " + var_name_->CppCode() + " ; \n";
}
/*!
    This is the UnParse method for the StringDecl class. When unparsed it has
   the form:
    Decl ::= 'string' varName ';'
*/
std::string StringDecl::UnParse() {
  return " string " + var_name_->UnParse() + " ; \n";
}

std::string StringDecl::CppCode() {
  return "string " + var_name_->CppCode() + " ; \n";
}
/*!
    This is the UnParse method for the BooleanDecl class.
    When unparsed it has the form:
    Decl ::= 'boolean' varName ';'
*/
std::string BooleanDecl::UnParse() {
  return "boolean " + var_name_->UnParse() + " ; \n";
}

std::string BooleanDecl::CppCode() {
  return "boolean " + var_name_->CppCode() + " ; \n";
}

MatrixDecl::MatrixDecl(VarName *var_name, Expr *expr) {
  var_name_ = var_name;
  expr_ = expr;
}
/*!
    This is the UnParse method for the MatrixDecl class.
    when unparsed it has the form:
    Decl ::= 'matrix' varName '=' Expr ';'
*/
std::string MatrixDecl::UnParse() {
  return " matrix " + var_name_->UnParse() + " = " + expr_->UnParse() + " ; \n";
}

std::string MatrixDecl::CppCode() {
  return "matrix " + var_name_->CppCode() + "( " + expr_->CppCode() + " ) ; \n";
}

LongMatrixDecl::LongMatrixDecl(VarName *var_name1, VarName *var_name2,
                               VarName *var_name3, Expr *expr1, Expr *expr2,
                               Expr *expr3) {
  var_name1_ = var_name1;
  var_name2_ = var_name2;
  var_name3_ = var_name3;
  expr1_ = expr1;
  expr2_ = expr2;
  expr3_ = expr3;
}

/*!
    This is the Unparse method for the LongMatrixDecl class.
    When unparsed it has the form:
    Decl ::= 'matrix' varName '[' Expr ':' Expr ']' varName ':' varName  '='
   Expr ';'
*/
std::string LongMatrixDecl::UnParse() {
  return "matrix " + var_name1_->UnParse() + " [ " + expr1_->UnParse() + " : " +
         expr2_->UnParse() + " ] " + var_name2_->UnParse() + " : " +
         var_name3_->UnParse() + " = " + expr3_->UnParse() + "; ";
}

std::string LongMatrixDecl::CppCode() {
  return "matrix " + var_name1_->CppCode() + "( " + expr1_->CppCode() + "," +
         expr2_->CppCode() + ") ; \n" + "for (int " + var_name2_->CppCode() +
         " = 0;" + var_name2_->CppCode() + " < " + expr1_->CppCode() + "; " +
         var_name2_->CppCode() + " ++ ) { \n" + "		for (int " +
         var_name3_->CppCode() + " = 0;" + var_name3_->CppCode() + " < " +
         expr2_->CppCode() + "; " + var_name3_->CppCode() + " ++ ) { \n" +
         " 	*(" + var_name1_->CppCode() + ".access(" +
         var_name2_->CppCode() + "," + var_name3_->CppCode() + ")) = " +
         expr3_->CppCode() + "	;} } \n";
}

// Expressions (Expr)

// Operator expression (productions 22-33)
BinaryOpExpr::BinaryOpExpr(Expr *expr1, std::string op, Expr *expr2) {
  expr1_ = expr1;
  operator_ = op;
  expr2_ = expr2;
}
/*!
    This is the UnParse method for the BinaryOpExpr class.
    When unparsed it can have one of the following forms:
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
std::string BinaryOpExpr::UnParse() {
  return expr1_->UnParse() + " " + operator_ + " " + expr2_->UnParse();
}

std::string BinaryOpExpr::CppCode() {
  return " (" + expr1_->CppCode() + " " + operator_ + " " + expr2_->CppCode() +
         ") ";
}

// MatrixRef expression
// Expr :== varName '[' Expr ':' Expr ']'
MatrixRefExpr::MatrixRefExpr(VarName *v, Expr *e1, Expr *e2) {
  var_name_ = v;
  expr1_ = e1;
  expr2_ = e2;
}
/*!
    This is the UnParse method for the MatrixRefExpr class.
    When unparsed it has the following form:
    Expr ::= varName '[' Expr ':' Expr ']'
*/
std::string MatrixRefExpr::UnParse() {
  return var_name_->UnParse() + " [ " + expr1_->UnParse() + " : " +
         expr2_->UnParse() + " ] ";
}

std::string MatrixRefExpr::CppCode() {
  return "*( " + var_name_->CppCode() + ".access(" + expr1_->CppCode() + ", " +
         expr2_->CppCode() + ")) ";
}
/*!
    This is the UnParse method for the BoolExpr class.
    When unparsed it has the following form:
    Expr ::= 'True' | 'False'
*/
std::string BoolExpr::UnParse() {
  std::ostringstream ss;
  ss << boolean_;
  return ss.str();
}

// std::string BoolExpr::CppCode() {  //Sifora did this one. Is it correct
// return " ( "" ) ";  //Sifora did this one. Correct? //uppercase, lowercase

//}

/*!
    This is the UnParse method for the VarNameExpr class.
    When unparsed it has the form:
    Expr ::= varName
*/
std::string VarNameExpr::UnParse() { return var_name_->UnParse(); }
std::string VarNameExpr::CppCode() { return var_name_->CppCode(); }

/*!
    This is the UnParse method for the ParenExpr class.
    When unparsed it has the form:
    Expr ::= '(' Expr ')'
*/
std::string ParenExpr::UnParse() { return " ( " + expr_->UnParse() + " ) "; }

std::string ParenExpr::CppCode() { return " ( " + expr_->CppCode() + " ) "; }
// NestedOrFunctionExpr
// Expr ::= VarName '(' Expr ')'
NestedOrFunctionExpr::NestedOrFunctionExpr(VarName *v, Expr *e) {
  var_name_ = v;
  expr_ = e;
}

/*!
    This is the UnParse method for the NestedOrFunctionExpr class.
    When unparsed it has the form:
    Expr ::= varName '(' Expr ')'
*/
std::string NestedOrFunctionExpr::UnParse() {
  return var_name_->UnParse() + " ( " + expr_->UnParse() + " ) ";
}

std::string NestedOrFunctionExpr::CppCode() {
  if (var_name_->CppCode() == "n_rows" || var_name_->CppCode() == "n_cols") {
    return expr_->CppCode() + "." + var_name_->CppCode() + "()";
  }
  return var_name_->CppCode() + " (" + expr_->CppCode() + " )";
}

// LetExpr
// Expr::= 'let' Stmts 'in' Expr 'end'
LetExpr::LetExpr(Stmts *ss, Expr *e) {
  stmts_ = ss;
  expr_ = e;
}
/*!
    This is the UnParse method for the LetExpr class.
    When unparsed it has the form:
    Expr ::= 'let' Stmts 'in' Expr 'end'
*/
std::string LetExpr::UnParse() {
  return " let \n" + stmts_->UnParse() + " in \n" + expr_->UnParse() + "\nend";
}

std::string LetExpr::CppCode() {
  return "({" + stmts_->CppCode() + expr_->CppCode() + "; })  ";
}
// If Expression
// Expr::= 'if' Expr 'then' Expr 'else' Expr
IfExpr::IfExpr(Expr *e1, Expr *e2, Expr *e3) {
  expr1_ = e1;
  expr2_ = e2;
  expr3_ = e3;
}
/*!
    This is the UnParse method for the IfExpr class.
    When unparsed it has the form:
    Expr ::= 'if' Expr 'then' Expr 'else' Expr
*/
std::string IfExpr::UnParse() {
  return " if " + expr1_->UnParse() + " then " + expr2_->UnParse() + " else " +
         expr3_->UnParse();
}
std::string IfExpr::CppCode() {
  return "( (" + expr1_->CppCode() + ") ? (" + expr2_->CppCode() + ") : " +
         expr3_->CppCode() + " )";
}

/*!
    This is the UnParse method for the NotExpr class.
    When unparsed it has the form:
    Expr ::= '!' Expr
*/
std::string NotExpr::UnParse() { return "!" + expr_->UnParse(); }

std::string NotExpr::CppCode() { return "! (" + expr_->CppCode() + ") "; }

/*!
    This is the UnParse method for the IntConstExpr class.
    When unparsed it has the form:
    Expr ::= integerConst
*/
std::string IntConstExpr::UnParse() { return const_int_; }

std::string IntConstExpr::CppCode() { return const_int_; }

/*!
    This is the UnParse method for the FloatConstExpr class.
    When unparsed it has the form:
    Expr ::= floatConst
*/
std::string FloatConstExpr::UnParse() { return const_float_; }

std::string FloatConstExpr::CppCode() { return const_float_; }

/*!
    This is the UnParse method for the StringConstExpr class.
    When UnParsed it has the form:
    Expr ::= stringConst
*/
std::string StringConstExpr::UnParse() { return string_const_; }

std::string StringConstExpr::CppCode() { return string_const_; }

} /* namespace ast */
} /* namespace fcal */
