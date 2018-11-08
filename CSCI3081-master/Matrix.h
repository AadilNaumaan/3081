/*******************************************************************************
 * Name            : Matrix.h
 * Project         : fcal
 * Module          : parser
 * Description     : a simple object for holding the result of a parse.
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Author : Erik Van Wyk
 * Modifications by: Dan Challou, John Harwell
 *
 ******************************************************************************/
#ifndef PROJECT_INCLUDE_MATRIX_H
#define PROJECT_INCLUDE_MATRIX_H

#include <stdlib.h>
#include <iostream>
#include <fstream>

/*! The Matrix class is declared here. It is composed of two ints defining the dimensions. We have a constructor and a copy constructor.  */
class matrix {
 public:
  matrix(int i, int j);
  matrix(const matrix &m);

  int n_rows();
  int n_cols();

  float *access(const int i, const int j) const;
  friend std::ostream &operator<<(std::ostream &os, matrix &m);
  friend matrix operator*(matrix &a, matrix &b);  // got rid of friend keyword
  matrix operator=(
      const matrix &m);  //, const matrix &m2); //got rid of friend keyword
  static matrix matrix_read(std::string filename);

 private:
  matrix() {}
  int rows;
  int cols;

  /* Your implementation of "data" may vary.  There are ways in
     which data can be an array of arrays and thus simplify the
     access method, at the cost of complicating the process of
     allocating space for data.  The choice is entirely up to
     you. */

     /*! We implemented data as a double pointer. */
  float **data;
};


#endif  // PROJECT_INCLUDE_MATRIX_H
