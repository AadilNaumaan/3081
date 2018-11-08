/*******************************************************************************
 * Name            : iter2scanner.h
 * Project         : fcal
 * Module          : scanner
 * Description     : Definitions of keywords/constants/etc.
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Authors : John Harwell, Dan Challou
 *
 ******************************************************************************/

#ifndef PROJECT_INCLUDE_SCANNER_H_
#define PROJECT_INCLUDE_SCANNER_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <regex.h>
#include <string>

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace fcal {
namespace scanner {
// Forward Declaration
int consume_whitespace_and_comments(regex_t *whiteSpace,
        regex_t *block_comment, regex_t *inline_comment, const char *text);
/*******************************************************************************
 * Constant Definitions
 ******************************************************************************/
/*
 * This enumerated type is used to keep track of what kind of
 * construct was matched.
 */
enum kTokenEnumType {
  kIntKwd,
  kFloatKwd,
  kBoolKwd,
  kTrueKwd,
  kFalseKwd,
  kStringKwd,
  kMatrixKwd,
  kLetKwd,
  kInKwd,
  kEndKwd,
  kIfKwd,
  kThenKwd,
  kElseKwd,
  kRepeatKwd,
  kWhileKwd,
  kPrintKwd,
  kToKwd,

  // Constants
  kIntConst,
  kFloatConst,
  kStringConst,

  // Names
  kVariableName,

  // Punctuation
  kLeftParen,
  kRightParen,
  kLeftCurly,
  kRightCurly,
  kLeftSquare,
  kRightSquare,
  kSemiColon,
  kColon,

  // Operators
  kAssign,
  kPlusSign,
  kStar,
  kDash,
  kForwardSlash,
  kLessThan,
  kLessThanEqual,
  kGreaterThan,
  kGreaterThanEqual,
  kEqualsEquals,
  kNotEquals,
  kAndOp,
  kOrOp,
  kNotOp,

  // Special terminal types
  kEndOfFile,
  kLexicalError
};
typedef enum kTokenEnumType TokenType;

class Token {
 public:
        Token();
        Token(std::string, TokenType, Token *);
        TokenType terminal();
        std::string lexeme();
        Token * next();
        void setTokenType(TokenType);
        void setLexeme_(std::string);
        void setNext(Token *);
 private:
        TokenType terminal_;
        std::string lexeme_;
        Token * next_;
};

class Scanner {
 public:
        Scanner();
        Token * Scan(const char *);
 private:
        regex_t* regex_array[44];
};

} /* namespace scanner */
} /* namespace fcal */

#endif  // PROJECT_INCLUDE_SCANNER_H_
