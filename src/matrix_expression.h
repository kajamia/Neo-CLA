#ifndef FILE_MATRIX_EXPRESSION_H
#define FILE_MATRIX_EXPRESSION_H

#include "matrix.h"

namespace ASC_bla
{

template <typename T>
class MatrixExpr
{/* 
  public:
    auto Upcast() const { return static_cast<const T&> (*this); }
    size_t height() const { return Upcast().height(); }
    size_t width() const { return Upcast().width(); }
    auto operator() (size_t i, size_t j) const { return Upcast()(i, j); } */
  };

}
 
#endif