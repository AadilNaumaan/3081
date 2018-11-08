/*******************************************************************************
 * Name            : Matrix.cc
 * Project         : fcal
 * Module          : parser
 * Description     : a simple object for holding the result of a parse.
 * Copyright       : 2017 CSCI3081W Staff. All rights reserved.
 * Original Author : Erik Van Wyk
 * Modifications by: Dan Challou, John Harwell
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "include/Matrix.h"
#include <fstream>
#include <iostream>

/*!We define the Matrix class here. We create a two-dimensional float array.*/
matrix::matrix(int rows_, int cols_) {
  rows = rows_;
  cols = cols_;

  data = new float *[rows];

  for (int i = 0; i < rows; i++) {
    data[i] = new float[cols];
  }
}

/*! These are the accessors for the rows and the columns of the matrix.*/
int matrix::n_rows() { return rows; }

int matrix::n_cols() { return cols; }

float *matrix::access(const int i, const int j) const { return &data[i][j]; }
/*! Here we overload the print operator for the Matrix class */
std::ostream &operator<<(std::ostream &os, matrix &m) {
  os << m.n_rows() << " " << m.n_cols() << std::endl;

  for (int i = 0; i < m.n_rows(); i++) {
    for (int j = 0; j < m.n_cols(); j++) {
      os << *(m.access(i, j)) << "  ";
    }
    os << std::endl;
  }

  return os;
}
/*! We overload the assign operator and the multiplication operator here. */
matrix operator*(matrix &a, matrix &b) {
  // matrix R = *this;
  int x = a.n_rows();
  int z = b.n_cols();
  if (b.n_rows() != a.n_cols()) {
    std::cout << "matrices dimensionses not compatible" << std::endl;
    exit(1);
  }
  int y = a.n_cols();
  matrix *R = new matrix(x, z);  // new matrix made from a
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < z; j++) {
      // R[i][j] = 0;
      *R->access(i, j) = 0;
      for (int k = 0; k < y; k++) {
        *R->access(i, j) += *a.access(i, k) * *b.access(k, j);
      } /* for(k..) */
    }   /* for(j..) */
  }     /* for(i..) */
  return (*R);
}
matrix matrix::operator=(const matrix &m) {
  rows = m.rows;
  cols = m.cols;

  // matrix *temp = new matrix(rows, cols);
  data = new float *[rows];

  for (int i = 0; i < rows; i++) {
    data[i] = new float[cols];
  }

  for (int j = 0; j < rows; j++) {
    for (int k = 0; k < cols; k++) {
      data[j][k] = *(m.access(j, k));
    }
  }

  return (*this);
}
matrix matrix::matrix_read(std::string filename) {
  int row;
  int col;
  std::ifstream fin;
  fin.open(filename.c_str());

  if (fin.fail()) {
    std::cout << "Failed to open file : " << filename << std::endl;
    exit(1);
  }

  if (fin.is_open()) {
    fin >> row >> col;
    matrix m = matrix(row, col);
    for (int i = 0; i < m.n_rows(); i++) {
      for (int j = 0; j < m.n_cols(); j++) {
        fin >> *(m.access(i, j));
      }
    }

    fin.close();
    return m;

  } else {
    std::cout << "READ FAIL" << std::endl;
    exit(1);
  }
}

/* Here we define the constructor for the Matrix class. */
matrix::matrix(const matrix &m) {
  rows = m.rows;
  cols = m.cols;

  data = new float *[rows];

  for (int i = 0; i < rows; i++) {
    data[i] = new float[cols];
  }

  for (int j = 0; j < rows; j++) {
    for (int k = 0; k < cols; k++) {
      data[j][k] = *(m.access(j, k));
    }
  }
}
