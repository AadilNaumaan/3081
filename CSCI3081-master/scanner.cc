/*******************************************************************************
 * Name            : scanner.cc
 * Project         : fcal
 * Module          : scanner
 * Description     : Implementation of scan method
 * Copyright 2017  : Aadil Naumaan and Sifora Tek-Lab
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/regex.h"
#include "include/scanner.h"

/*******************************************************************************
 * Namespaces
 ******************************************************************************/
namespace fcal {
namespace scanner {
/*! Scanner::Scanner() is the constructor. It creates an array of regex pointers
and populates that array with calls to make_regex with various regular
expressions for the different terminal types. */
Scanner::Scanner() {
  // Keyword TokenTypes
  regex_array[0] = make_regex("^int");
  regex_array[1] = make_regex("^float");
  regex_array[2] = make_regex("^boolean");
  regex_array[3] = make_regex("^True");
  regex_array[4] = make_regex("^False");
  regex_array[5] = make_regex("^string");
  regex_array[6] = make_regex("^matrix");
  regex_array[7] = make_regex("^let");
  regex_array[8] = make_regex("^in");
  regex_array[9] = make_regex("^end");
  regex_array[10] = make_regex("^if");
  regex_array[11] = make_regex("^then");
  regex_array[12] = make_regex("^else");
  regex_array[13] = make_regex("^repeat");
  regex_array[14] = make_regex("^while");
  regex_array[15] = make_regex("^print");
  regex_array[16] = make_regex("^to");

  // Constant TokenTypes
  regex_array[17] = make_regex("^[0-9]+");
  regex_array[18] = make_regex("^[0-9]+\\.[0-9]+");
  regex_array[19] = make_regex("^\"(\\.|[^\"])*\"");

  // Variable TokenType
  regex_array[20] = make_regex("^[a-zA-Z0-9_]+");

  // Punctuation TokenTypes
  regex_array[21] = make_regex("^\\(");
  regex_array[22] = make_regex("^\\)");
  regex_array[23] = make_regex("^\\{");
  regex_array[24] = make_regex("^\\}");
  regex_array[25] = make_regex("^\\[");
  regex_array[26] = make_regex("^\\]");
  regex_array[27] = make_regex("^;");
  regex_array[28] = make_regex("^:");

  // Operator Tokentypes
  regex_array[29] = make_regex("^=");
  regex_array[30] = make_regex("^\\+");
  regex_array[31] = make_regex("^\\*");
  regex_array[32] = make_regex("^-");
  regex_array[33] = make_regex("^/");
  regex_array[34] = make_regex("^<");
  regex_array[35] = make_regex("^<=");
  regex_array[36] = make_regex("^>");
  regex_array[37] = make_regex("^>=");
  regex_array[38] = make_regex("^==");
  regex_array[39] = make_regex("^!=");
  regex_array[40] = make_regex("^&&");
  regex_array[41] = make_regex("^\\|\\|");
  regex_array[42] = make_regex("^!");

  // Special Terminal Types
  regex_array[43] = make_regex("$\\0");
}
/*! Default Token constructor. Instantiate lexeme_ to empty string,
terminal_ to lexical error token type, and next_ (which is next token) to
NULL */
Token::Token() {
  lexeme_ = "";
  terminal_ = kLexicalError;
  next_ = NULL;
}

/*! This constructor requires three parameters: lexeme_, terminal_, and next_
    \param Lexeme_ the lexeme (String) for the token
    \param terminal_ the terminal as a TokenType
    \param next_ the pointer to the next Token
*/
Token::Token(std::string a, TokenType t, Token* s) {
  lexeme_ = a;
  terminal_ = t;
  next_ = s;
}
/*! This returns the TokenType terminal_ */
TokenType Token::terminal(void) { return terminal_; }

/*! This returns the token string lexeme_ */
std::string Token::lexeme(void) { return lexeme_; }

/*! This returns the pointer to the next token next_ */
Token* Token::next(void) { return next_; }

/*! This function sets the terminal_ of the token objects
  \param terminal_ the terminal type
*/
void Token::setTokenType(TokenType type) { terminal_ = type; }

/*! This function sets the lexeme_ of the token object
  \param Lexeme_ the string representing the token */
void Token::setLexeme_(std::string lex) { lexeme_ = lex; }

/*! This function sets the next token of the token object
  \param next_ the pointer to the next token object */
void Token::setNext(Token* nextToken) { next_ = nextToken; }

/*! Create the compiled regular expressions. */
regex_t* white_space = make_regex("^[\n\t\r ]+");
regex_t* block_comment = make_regex("^/\\*([^\\*]|\\*+[^\\*/])*\\*+/");
regex_t* inline_comment = make_regex("^//[^\n\r]*");

Token* Scanner::Scan(const char* text) {
  int num_matched_chars;
  /*! Consume leading white space and comments */
  num_matched_chars = consume_whitespace_and_comments(
      white_space, block_comment, inline_comment, text);

  text = text + num_matched_chars;

  int max_num_matched_chars = 0;

  Token* head_token = new Token;
  Token* current_token = new Token;
  current_token = head_token;
  kTokenEnumType match_type;
  while (text[0] != '\0') {
    max_num_matched_chars = 0;
    match_type = kLexicalError;

    for (int i = 0; i < 44; i++) {
      num_matched_chars = match_regex(regex_array[i], text);
      if (num_matched_chars > max_num_matched_chars) {
        max_num_matched_chars = num_matched_chars;
        match_type = kTokenEnumType(i);
      }
    }

    if (max_num_matched_chars == 0) {
      max_num_matched_chars = 1;
    }
    std::string str(text, max_num_matched_chars);

    Token* new_token;
    new_token = new Token;
    current_token->setTokenType(match_type);
    current_token->setLexeme_(str);
    current_token->setNext(new_token);
    current_token = new_token;

    text = text + max_num_matched_chars;
    num_matched_chars = consume_whitespace_and_comments(
        white_space, block_comment, inline_comment, text);
    text = text + num_matched_chars;
  }
  current_token->setTokenType(kEndOfFile);
  current_token->setNext(NULL);

  return head_token;
}

int consume_whitespace_and_comments(regex_t* white_space,
                                    regex_t* block_comment,
                                    regex_t* inline_comment, const char* text) {
  int num_matched_chars = 0;
  int total_num_matched_chars = 0;
  int still_consuming_white_space;

  do {
    still_consuming_white_space = 0;  // exit loop if not reset by a match

    /*! Try to match white space */
    num_matched_chars = match_regex(white_space, text);
    total_num_matched_chars += num_matched_chars;
    if (num_matched_chars > 0) {
      text = text + num_matched_chars;
      still_consuming_white_space = 1;
    }

    /*! Try to match block comments */
    num_matched_chars = match_regex(block_comment, text);
    total_num_matched_chars += num_matched_chars;
    if (num_matched_chars > 0) {
      text = text + num_matched_chars;
      still_consuming_white_space = 1;
    }

    /*! Try to match inline comments */
    num_matched_chars = match_regex(inline_comment, text);
    total_num_matched_chars += num_matched_chars;
    if (num_matched_chars > 0) {
      text = text + num_matched_chars;
      still_consuming_white_space = 1;
    }
  } while (still_consuming_white_space);

  return total_num_matched_chars;
} /* consume_whitespace_and_comments() */

} /* namespace scanner */
} /* namespace fcal */
