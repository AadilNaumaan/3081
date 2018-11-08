/*******************************************************************************
 * Name            : parser.cc
 * Project         : fcal
 * Module          : parser
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Author : Erik Van Wyk
 * Modifications by: Aadil Naumaan and Sifora Tek-Lab
 *
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "include/parser.h"
#include <assert.h>
#include <stdio.h>
#include "include/ext_token.h"
#include "include/scanner.h"
#include "include/ast.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace fcal {
namespace parser {

/*******************************************************************************
 * Constructors/Destructor
 ******************************************************************************/
Parser::~Parser() {
  if (scanner_) delete scanner_;

  scanner::ExtToken *to_delete1;
  curr_token_ = tokens_;
  while (curr_token_) {
    to_delete1 = curr_token_;
    curr_token_ = curr_token_->next();
    delete to_delete1;
  } /* while() */

  scanner::Token *curr_scanner_token = stokens_;
  scanner::Token *to_delete2;
  while (curr_scanner_token) {
    to_delete2 = curr_scanner_token;
    curr_scanner_token = curr_scanner_token->next();
    delete to_delete2;
  } /* while() */
} /* Parser::~Parser() */

ParseResult Parser::Parse(const char *text) {
  assert(text != NULL);

  ParseResult pr;
  try {
    scanner_ = new scanner::Scanner();
    stokens_ = scanner_->Scan(text);
    tokens_ = tokens_->ExtendTokenList(this, stokens_);

    assert(tokens_ != NULL);
    curr_token_ = tokens_;
    pr = ParseProgram();
  } catch (std::string errMsg) {
    pr.ok(false);
    pr.errors(errMsg);
    pr.ast(NULL);
  }
  return pr;
} /* Parser::parse() */

/*
 * parse methods for non-terminal symbols
 * --------------------------------------
 */

// Program
ParseResult Parser::ParseProgram() {
  // std::cout<<"\nParse program:"<<curr_token_->lexeme()<<std::endl;
  ParseResult pr;
  // root
  // Program ::= varName '(' ')' '{' Stmts '}'
  match(scanner::kVariableName);
  std::string name(prev_token_->lexeme());
  ast::VarName *varname = new ast::VarName(name);
  match(scanner::kLeftParen);
  match(scanner::kRightParen);
  match(scanner::kLeftCurly);
  ParseResult pr_stmts = parse_stmts();
  ast::Stmts *s = NULL;
  if (pr_stmts.ast()) {
    s = dynamic_cast<ast::Stmts *>(pr_stmts.ast());
    if (!s) throw((std::string) "Bad cast of State in parseProgram");
  }
  match(scanner::kRightCurly);
  match(scanner::kEndOfFile);

  pr.ast(new ast::Root(varname, s));
  return pr;
} /* Parser::ParseProgram() */

// MatrixDecl
// identical purpose of parse_decl, handles special matrix syntax.
ParseResult Parser::parse_matrix_decl() {
  // std::cout <<"\nParsematrixdecl:"<<curr_token_->lexeme()<<std::endl;
  ParseResult pr;
  match(scanner::kMatrixKwd);
  match(scanner::kVariableName);
  std::string name1(prev_token_->lexeme());
  ast::VarName *varname1 = new ast::VarName(name1);

  // Decl ::= 'matrix' varName '[' Expr ':' Expr ']' varName ':' varName  '='
  // Expr ';'
  if (attempt_match(scanner::kLeftSquare)) {
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kColon);
    ParseResult exprPr2 = parse_expr(0);
    ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
    match(scanner::kRightSquare);
    match(scanner::kVariableName);
    std::string name2(prev_token_->lexeme());
    ast::VarName *varname2 = new ast::VarName(name2);
    match(scanner::kColon);
    match(scanner::kVariableName);
    std::string name3(prev_token_->lexeme());
    ast::VarName *varname3 = new ast::VarName(name3);
    match(scanner::kAssign);
    ParseResult exprPr3 = parse_expr(0);
    ast::Expr *expr3 = dynamic_cast<ast::Expr *>(exprPr3.ast());
    pr.ast(new ast::LongMatrixDecl(varname1, varname2, varname3, expr1, expr2,
                                   expr3));
  } else if (attempt_match(scanner::kAssign)) {
    // Decl ::= 'matrix' varName '=' Expr ';'
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    pr.ast(new ast::MatrixDecl(varname1, expr1));
  } else {
    throw((std::string) "Bad Syntax of Matrix Decl in in parseMatrixDecl");
  }

  match(scanner::kSemiColon);

  return pr;
}
// standardDecl
// Decl ::= integerKwd varName | floatKwd varName | stringKwd varName
ParseResult Parser::parse_standard_decl() {
  ParseResult pr;
  int condition;
  if (attempt_match(scanner::kIntKwd)) {  // Type ::= intKwd
    condition = 0;
  } else if (attempt_match(scanner::kFloatKwd)) {  // Type ::= floatKwd
    condition = 1;
  } else if (attempt_match(scanner::kStringKwd)) {  // Type ::= stringKwd
    condition = 2;
  } else if (attempt_match(scanner::kBoolKwd)) {  // Type ::= boolKwd
    condition = 3;
  } else {
    throw((std::string) "Bad Syntax of Type Decl in parse_standard_decl");
  }
  match(scanner::kVariableName);
  std::string name(prev_token_->lexeme());
  ast::VarName *varname = new ast::VarName(name);
  match(scanner::kSemiColon);
  if (condition == 0)
    pr.ast(new ast::IntDecl(varname));
  else if (condition == 1)
    pr.ast(new ast::FloatDecl(varname));
  else if (condition == 2)
    pr.ast(new ast::StringDecl(varname));
  else if (condition == 3)
    pr.ast(new ast::BooleanDecl(varname));
  return pr;
}

// Decl
ParseResult Parser::parse_decl() {
  // std::cout<<"\n Success: parse_decl:"<<curr_token_->lexeme()<<std::endl;
  ParseResult pr;
  // Decl :: matrix variableName ....
  if (next_is(scanner::kMatrixKwd)) {
    pr = parse_matrix_decl();
  } else {
    // Decl ::= Type variableName semiColon
    pr = parse_standard_decl();
  }
  return pr;
}

// Stmts
ParseResult Parser::parse_stmts() {
  // std::cout<<"\n Success: parse_stmts:"<<curr_token_->lexeme()<<std::endl;
  ParseResult pr;
  if (!next_is(scanner::kRightCurly) && !next_is(scanner::kInKwd)) {
    // Stmts ::= Stmt Stmts
    ParseResult pr_stmt = parse_stmt();
    ast::Stmt *stmt = NULL;
    if (pr_stmt.ast()) {
      stmt = dynamic_cast<ast::Stmt *>(pr_stmt.ast());
      if (!stmt) throw((std::string) "Bad cast of stmt in parse_stmts");
    }
    ParseResult pr_stmts = parse_stmts();
    ast::Stmts *stmts = NULL;
    if (pr_stmts.ast()) {
      stmts = dynamic_cast<ast::Stmts *>(pr_stmts.ast());
      if (!stmts) throw((std::string) "Bad cast of stmts in parse_stmts");
    }
    pr.ast(new ast::StmtsSeq(stmt, stmts));
  } else {
    // Stmts ::=
    // nothing to match.k
    pr.ast(new ast::EmptyStmts());
  }
  return pr;
}

// Stmt
ParseResult Parser::parse_stmt() {
  // std::cout<<"\n Success: parse_stmt:"<<curr_token_->lexeme()<<std::endl;
  ParseResult pr;

  // Stmt ::= Decl
  if (next_is(scanner::kIntKwd) || next_is(scanner::kFloatKwd) ||
      next_is(scanner::kMatrixKwd) || next_is(scanner::kStringKwd) ||
      next_is(scanner::kBoolKwd)) {
    ParseResult declPr = parse_decl();
    ast::Decl *decl = dynamic_cast<ast::Decl *>(declPr.ast());
    pr.ast(new ast::DeclStmt(decl));
  } else if (attempt_match(scanner::kLeftCurly)) {
    // Stmt ::= '{' Stmts '}'
    ParseResult prStmts = parse_stmts();
    ast::Stmts *stmts = dynamic_cast<ast::Stmts *>(prStmts.ast());
    pr.ast(new ast::StmtStmts(stmts));
    match(scanner::kRightCurly);
  } else if (attempt_match(scanner::kIfKwd)) {
    // Stmt ::= 'if' '(' Expr ')' Stmt
    // Stmt ::= 'if' '(' Expr ')' Stmt 'else' Stmt
    match(scanner::kLeftParen);
    ParseResult exprPr = parse_expr(0);
    ast::Expr *expr = dynamic_cast<ast::Expr *>(exprPr.ast());
    match(scanner::kRightParen);
    ParseResult stmtPr1 = parse_stmt();
    ast::Stmt *stmt1 = dynamic_cast<ast::Stmt *>(stmtPr1.ast());

    if (attempt_match(scanner::kElseKwd)) {
      ParseResult stmtPr2 = parse_stmt();
      ast::Stmt *stmt2 = dynamic_cast<ast::Stmt *>(stmtPr2.ast());
      pr.ast(new ast::IfElseStmt(expr, stmt1, stmt2));
    } else {
      pr.ast(new ast::IfStmt(expr, stmt1));
    }
  } else if (attempt_match(scanner::kVariableName)) {
    ast::Expr *expr2;
    ast::Expr *expr3;
    /*
     * Stmt ::= varName '=' Expr ';'  | varName '[' Expr ':' Expr ']'
     * '=' Expr ';'
     */
    std::string name(prev_token_->lexeme());
    ast::VarName *varname = new ast::VarName(name);
    bool leftSquare = false;
    if (attempt_match(scanner::kLeftSquare)) {
      leftSquare = true;
      ParseResult exprPr2 = parse_expr(0);
      ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
      match(scanner::kColon);
      ParseResult exprPr3 = parse_expr(0);
      ast::Expr *expr3 = dynamic_cast<ast::Expr *>(exprPr3.ast());
      match(scanner::kRightSquare);
    }
    match(scanner::kAssign);
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kSemiColon);
    if (leftSquare)
      pr.ast(new ast::AssignMatrixStmt(varname, expr2, expr3, expr1));
    else
      pr.ast(new ast::AssignStmt(varname, expr1));

  } else if (attempt_match(scanner::kPrintKwd)) {
    // Stmt ::= 'print' '(' Expr ')' ';'
    match(scanner::kLeftParen);
    ParseResult exprPr = parse_expr(0);
    ast::Expr *expr = dynamic_cast<ast::Expr *>(exprPr.ast());
    match(scanner::kRightParen);
    match(scanner::kSemiColon);
    pr.ast(new ast::PrintStmt(expr));
  } else if (attempt_match(scanner::kRepeatKwd)) {
    // Stmt ::= 'repeat' '(' varName '=' Expr 'to' Expr ')' Stmt
    match(scanner::kLeftParen);
    match(scanner::kVariableName);
    std::string name(prev_token_->lexeme());
    ast::VarName *varname = new ast::VarName(name);
    match(scanner::kAssign);
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kToKwd);
    ParseResult exprPr2 = parse_expr(0);
    ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
    match(scanner::kRightParen);
    ParseResult pr_stmt = parse_stmt();
    ast::Stmt *stmt = NULL;
    if (pr_stmt.ast()) {
      stmt = dynamic_cast<ast::Stmt *>(pr_stmt.ast());
      if (!stmt) throw((std::string) "Bad cast of stmt in parse_stmt");
    }
    pr.ast(new ast::RepeatStmt(varname, expr1, expr2, stmt));
  } else if (attempt_match(scanner::kWhileKwd)) {
    // Stmt ::= 'while' '(' Expr ')' Stmt
    match(scanner::kLeftParen);
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kRightParen);
    ParseResult pr_stmt = parse_stmt();
    ast::Stmt *stmt = NULL;
    if (pr_stmt.ast()) {
      stmt = dynamic_cast<ast::Stmt *>(pr_stmt.ast());
      if (!stmt) throw((std::string) "Bad cast of stmt in parse_stmt");
    }
    pr.ast(new ast::WhileStmt(expr1, stmt));
  } else if (attempt_match(scanner::kSemiColon)) {
    // Stmt ::= ';
    // parsed a skip
    pr.ast(new ast::SemiStmt());
  } else {
    throw(make_error_msg(curr_token_->terminal()) +
          " while parsing a statement");
  }
  // Stmt ::= variableName assign Expr semiColon
  return pr;
}

// Expr
ParseResult Parser::parse_expr(int rbp) {
  // std::cout<<"\n Success: parse_expr:"<<curr_token_->lexeme()<<std::endl;
  /* Examine current token, without consuming it, to call its
     associated parse methods.  The ExtToken objects have 'nud' and
     'led' methods that are dispatchers that call the appropriate
     parse methods.*/
  ParseResult left = curr_token_->nud();

  while (rbp < curr_token_->lbp()) {
    left = curr_token_->led(left);
  }

  return left;
}

/*
 * parse methods for Expr productions
 * ----------------------------------
 */

// Expr ::= trueKwd
ParseResult Parser::parse_true_kwd() {
  ParseResult pr;
  match(scanner::kTrueKwd);
  pr.ast(new ast::BoolExpr(true));
  return pr;
}

// Expr ::= falseKwd
ParseResult Parser::parse_false_kwd() {
  ParseResult pr;
  match(scanner::kFalseKwd);
  pr.ast(new ast::BoolExpr(false));
  return pr;
}

// Expr ::= intConst
ParseResult Parser::parse_int_const() {
  ParseResult pr;
  match(scanner::kIntConst);
  std::string intConst(prev_token_->lexeme());
  pr.ast(new ast::IntConstExpr(intConst));
  return pr;
}

// Expr ::= floatConst
ParseResult Parser::parse_float_const() {
  ParseResult pr;
  match(scanner::kFloatConst);
  std::string floatConst(prev_token_->lexeme());
  pr.ast(new ast::FloatConstExpr(floatConst));
  return pr;
}

// Expr ::= stringConst
ParseResult Parser::parse_string_const() {
  ParseResult pr;
  match(scanner::kStringConst);
  std::string stringConst(prev_token_->lexeme());
  pr.ast(new ast::StringConstExpr(stringConst));
  return pr;
}

// Expr ::= variableName .....
ParseResult Parser::parse_variable_name() {
  ParseResult pr;
  match(scanner::kVariableName);
  std::string name(prev_token_->lexeme());
  ast::VarName *varname = new ast::VarName(name);
  if (attempt_match(scanner::kLeftSquare)) {
    // Expr ::= varName '[' Expr ':' Expr ']'
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kColon);
    ParseResult exprPr2 = parse_expr(0);
    ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
    match(scanner::kRightSquare);
    pr.ast(new ast::MatrixRefExpr(varname, expr1, expr2));
  } else if (attempt_match(scanner::kLeftParen)) {
    // Expr ::= varableName '(' Expr ')'
    ParseResult exprPr1 = parse_expr(0);
    ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
    match(scanner::kRightParen);
    pr.ast(new ast::NestedOrFunctionExpr(varname, expr1));
  } else {
    // variable
    pr.ast(new ast::VarNameExpr(varname));
  }
  return pr;
}

// Expr ::= leftParen Expr rightParen
ParseResult Parser::parse_nested_expr() {
  ParseResult pr;
  match(scanner::kLeftParen);
  pr.ast(new ast::ParenExpr(dynamic_cast<ast::Expr *>(parse_expr(0).ast())));
  match(scanner::kRightParen);
  return pr;
}

// Expr ::= 'if' Expr 'then' Expr 'else' Expr
ParseResult Parser::parse_if_expr() {
  ParseResult pr;

  match(scanner::kIfKwd);
  ParseResult exprPr1 = parse_expr(0);
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
  match(scanner::kThenKwd);
  ParseResult exprPr2 = parse_expr(0);
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  match(scanner::kElseKwd);
  ParseResult exprPr3 = parse_expr(0);
  ast::Expr *expr3 = dynamic_cast<ast::Expr *>(exprPr3.ast());
  pr.ast(new ast::IfExpr(expr1, expr2, expr3));
  return pr;
}

// Expr ::= 'let' Stmts 'in' Expr 'end'
ParseResult Parser::parse_let_expr() {
  ParseResult pr;
  match(scanner::kLetKwd);
  ParseResult pr_stmts = parse_stmts();
  ast::Stmts *stmts = NULL;
  if (pr_stmts.ast()) {
    stmts = dynamic_cast<ast::Stmts *>(pr_stmts.ast());
    if (!stmts) throw((std::string) "Bad cast of stmts in parse_let_expr");
  }
  match(scanner::kInKwd);
  ParseResult exprPr1 = parse_expr(0);
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(exprPr1.ast());
  match(scanner::kEndKwd);
  pr.ast(new ast::LetExpr(stmts, expr1));
  return pr;
}

// Expr ::= '!' Expr
ParseResult Parser::parse_not_expr() {
  ParseResult pr;
  match(scanner::kNotOp);
  pr.ast(new ast::NotExpr(dynamic_cast<ast::Expr *>(parse_expr(0).ast())));
  return pr;
}

// Expr ::= Expr plusSign Expr
ParseResult Parser::parse_addition(ParseResult prLeft) {
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(prLeft.ast());
  ParseResult pr;
  match(scanner::kPlusSign);
  ParseResult exprPr2 = parse_expr(prev_token_->lbp());
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  pr.ast(new ast::BinaryOpExpr(expr1, "+", expr2));
  return pr;
}

// Expr ::= Expr star Expr
ParseResult Parser::parse_multiplication(ParseResult prLeft) {
  // parser has already matchekD left expression
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(prLeft.ast());
  ParseResult pr;
  match(scanner::kStar);
  ParseResult exprPr2 = parse_expr(prev_token_->lbp());
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  pr.ast(new ast::BinaryOpExpr(expr1, "*", expr2));
  return pr;
}

// Expr ::= Expr dash Expr
ParseResult Parser::parse_subtraction(ParseResult prLeft) {
  // parser has already matchekD left expression
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(prLeft.ast());
  ParseResult pr;
  match(scanner::kDash);
  ParseResult exprPr2 = parse_expr(prev_token_->lbp());
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  pr.ast(new ast::BinaryOpExpr(expr1, "-", expr2));
  return pr;
}

// Expr ::= Expr forwardSlash Expr
ParseResult Parser::parse_division(ParseResult prLeft) {
  // parser has already matchekD left expression
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(prLeft.ast());
  ParseResult pr;
  match(scanner::kForwardSlash);
  ParseResult exprPr2 = parse_expr(prev_token_->lbp());
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  pr.ast(new ast::BinaryOpExpr(expr1, "/", expr2));
  return pr;
}

// Expr ::= Expr equalEquals Expr
// Expr ::= Expr lessThanEquals Expr
// Expr ::= Expr greaterThanEquals Expr
// Expr ::= Expr notEquals Expr
// Expr ::= Expr leftAngle Expr
// Expr ::= Expr rightAngle Expr
/* Notice that for relational operators we use just one parse
   function.  This shows another possible means for implementing
   expressions, as opposed to the method used for arithmetic
   expressions in which each operation has its own parse method.  It
   will depend on what we do in iteration 3 in building an abstract
   syntax tree to decide which method is better.
*/
ParseResult Parser::parse_relational_expr(ParseResult prLeft) {
  // parser has already matchekD left expression
  ast::Expr *expr1 = dynamic_cast<ast::Expr *>(prLeft.ast());
  ParseResult pr;

  next_token();
  // just advance token, since examining it in parse_expr caused
  // this method being called.
  std::string op = prev_token_->lexeme();

  ParseResult exprPr2 = parse_expr(prev_token_->lbp());
  ast::Expr *expr2 = dynamic_cast<ast::Expr *>(exprPr2.ast());
  pr.ast(new ast::BinaryOpExpr(expr1, op, expr2));
  return pr;
}

// Helper function used by the parser.

void Parser::match(const scanner::TokenType &tt) {
  if (!attempt_match(tt)) {
    throw(make_error_msg_expected(tt));
  }
}

bool Parser::attempt_match(const scanner::TokenType &tt) {
  if (curr_token_->terminal() == tt) {
    next_token();
    return true;
  }
  return false;
}

bool Parser::next_is(const scanner::TokenType &tt) {
  return curr_token_->terminal() == tt;
}

void Parser::next_token() {
  if (curr_token_ == NULL) {
    throw(std::string(
        "Internal Error: should not call nextToken in unitialized state"));
  } else if (curr_token_->terminal() == scanner::kEndOfFile &&
             curr_token_->next() == NULL) {
    prev_token_ = curr_token_;
  } else if (curr_token_->terminal() != scanner::kEndOfFile &&
             curr_token_->next() == NULL) {
    throw(make_error_msg("Error: tokens end with endOfFile"));
  } else {
    prev_token_ = curr_token_;
    curr_token_ = curr_token_->next();
  }
}

std::string Parser::terminal_description(const scanner::TokenType &terminal) {
  scanner::Token *dummy_token = new scanner::Token("", terminal, NULL);
  scanner::ExtToken *dummy_ext_token = new scanner::ExtToken(this, dummy_token);
  std::string s =
      dummy_ext_token->ExtendToken(this, dummy_token)->description();
  delete dummy_token;
  delete dummy_ext_token;
  return s;
}

std::string Parser::make_error_msg_expected(
    const scanner::TokenType &terminal) {
  std::string s = (std::string) "Expected " + terminal_description(terminal) +
                  " but found " + curr_token_->description();
  return s;
}

std::string Parser::make_error_msg(const scanner::TokenType &terminal) {
  std::string s = "Unexpected symbol " + terminal_description(terminal);
  return s;
}

std::string Parser::make_error_msg(const char *msg) { return msg; }

} /* namespace parser */
} /* namespace fcal */
