#ifndef FILE_MATRIX_H
#define FILE_MATRIX_H

#include <iostream>
#include <initializer_list>
#include <stdexcept>

#include "vector.h"
#include "matrix_expression.h"
#include "expression.h"


namespace ASC_bla {

// choice of row or column major, for template
enum ORDERING { RowMajor, ColMajor };


template <typename T, ORDERING ORD>
class MatrixView : public MatrixExpr<MatrixView<T, ORD> >
{
 protected:
  size_t height_, width_, dist_;
  T *data_;

  // TODO Inverse(MatrixView) (outside of MatrixView)

 public:
  // constructor
  MatrixView(size_t height, size_t width, T *data)
    : height_(height), width_(width), data_(data) {
    if constexpr (ORD == RowMajor){
      dist_ = width;
    }else{
      dist_ = height;
    }
    }
  
  // constructor with dist argument
  MatrixView(size_t height, size_t width, size_t dist, T *data)
    : height_(height), width_(width), dist_(dist), data_(data) {;};
  
  // copy constructor
  MatrixView(const MatrixView<T, ORD> & A)
    : height_(A.height()), width_(A.width()), dist_(A.dist_), data_(A.data_) {;}

  // assignment operator
  template <typename TB>
  MatrixView &operator=(const MatrixExpr<TB> & M) {
    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        if constexpr (ORD == RowMajor) {
          data_[dist_ * i + j] = M(i, j);
        } else {
          data_[dist_ * j + i] = M(i, j);
        }
      }
    }
    return *this;
  }

  // assignment operator
  template <typename TB, ORDERING ORDB>
  MatrixView &operator=(MatrixView<TB, ORDB> M) {
    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        if constexpr (ORD == RowMajor) {
          data_[dist_ * i + j] = M(i, j);
        } else {
          data_[dist_ * j + i] = M(i, j);
        }
      }
    }
    return *this;
  }

  // set all matrix components to scal
  MatrixView & operator= (T scal) {
    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        if constexpr (ORD == RowMajor) {
          data_[dist_ * i + j] = scal;
        } else {
          data_[dist_ * j + i] = scal;
        }
      }
    }
    return *this;
  }

  // returns new view to current object
  auto View() const { return MatrixView(height_, width_, data_); }

  // returns dimensions of matrix
  size_t height() const { return this->height_; };
  size_t width() const { return this->width_; };

  T* Data(){return data_;}
  
  // returns dist_
  size_t Dist() const {return dist_;}

  // round bracket access operator
  T &operator()(size_t i, size_t j) {
    // constexpr evaluates the if-statement at compile time
    if constexpr (ORD == RowMajor) {
      return data_[i * dist_ + j];
    }
    if constexpr (ORD == ColMajor) {
      return data_[j * dist_ + i];
    }
  }

  // round bracket access operator, constant version
  const T &operator()(size_t i, size_t j) const {
    // constexpr evaluates the if-statement at compile time
    if constexpr (ORD == RowMajor) {
      return data_[i * dist_ + j];
    }
    if constexpr(ORD == ColMajor) {
      return data_[j * dist_ + i];
    }
  }

  // transposed view of matrix
  auto transposed() const{
    if constexpr (ORD == RowMajor){
      return MatrixView<T, ColMajor> (width_, height_, dist_, data_);
    }
    else{
      return MatrixView<T, RowMajor> (width_, height_, dist_, data_);
    }
  }

  // returns row i as a VectorView
  auto Row(size_t i){
    if constexpr (ORD == RowMajor){
      return VectorView<T, std::integral_constant<size_t,1> > (width_, data_ + i*dist_);
    }
    else{
      return VectorView<T, size_t> (width_, dist_, (data_ + i));
    }
  }

  // returns column j as VectorView
  auto Col(size_t j){
    if constexpr  (ORD == ColMajor){
      return VectorView<T, std::integral_constant<size_t,1> > (height_, data_ + j*dist_);
    }
    else{
      return VectorView<T, size_t> (height_, dist_, (data_ + j));
    }
  }

  // returns the submatrix from row start with given height
  MatrixView<T, ORD> Rows(size_t start, size_t height){

    /* // error handling
    if (start > stop){ throw std::out_of_range("row start index too large")};
    if (stop >= width()){ throw std::out_of_range("row stop index out of range")}; */
    if constexpr (ORD == RowMajor){
      return MatrixView<T, ORD> (height, width_, dist_, (data_ + start*width_));
    }
    else{
      return MatrixView<T, ORD> (height, width_, height_, (data_ + start));
    }
  }

  // returns the submatrix from column start with given width
  MatrixView<T, ORD> Cols(size_t start, size_t width){
    if constexpr (ORD == RowMajor){
      return MatrixView<T, ORD> (height_, width, width_, (data_ + start));
    }
    else{
      return MatrixView<T, ORD> (height_, width, dist_, (data_ + start*height_));
    }
  }

  void swapcols(size_t i, size_t j){      //(*this) - Matrix (M)
  // row-wise swap for efficiency
    T tmp;
    for (size_t k = 0; k < height_; k++) {
      tmp = (*this)(k, i);
      (*this)(k, i) = (*this)(k, j);
      (*this)(k, j) = tmp;

    }
  }

  template <typename TB, ORDERING ORDB>
  void deepcopy(const MatrixView<TB, ORDB> & M){
    if (width_ != M.width()) throw width_;
    if (height_ != M.height()) throw height_;

    for (size_t i = 0; i < height_; i++) {
      for (size_t j = 0; j < width_; j++) {
        (*this)(i, j) = M(i, j);
      }
    }
  }

};


template <typename T, ORDERING ORD = RowMajor>
class Matrix : public MatrixView<T, ORD> {
  typedef MatrixView<T, ORD> BASE;
  using BASE::data_;
  using BASE::height_;
  using BASE::width_;
  using BASE::dist_;

 public:
  // constructor
  Matrix(size_t height, size_t width)
    : MatrixView<T, ORD> (height, width, new T[height*width]) {;}

  // copy constructor
  Matrix (const Matrix & A)
    : Matrix(A.height(), A.width())
  {
    *this=A;
  }

  // move constructor
  Matrix (Matrix && A)
      : MatrixView<T, ORD> (0, 0, nullptr)
    {
      std::swap(width_, A.width_);
      std::swap(height_, A.height_);
      std::swap(dist_, A.dist_);
      std::swap(data_, A.data_);
    }

  // constructor from MatrixExpr
  template <typename TB>
  Matrix (const MatrixExpr<TB> & B)
    : Matrix<T, ORD> (B.height(), B.width()) {
    
    *this = B;

    // set dist_
    if (ORD == RowMajor){
      dist_ = width_;
    }else{
      dist_ = height_;
    }
  }
  
  // initializer list constructor
  Matrix (size_t height, size_t width, std::initializer_list<T> list)
    : MatrixView<T, ORD> (height, width, new T[list.size()]) {
    // check if list has the right size
    if (list.size() != height_*width_){
      throw list.size();
      return;
    }else{
      // copy list
      for (size_t i = 0; i < list.size(); i++){
        data_[i] = list.begin()[i];
      }
    }

    // set dist_
    if constexpr (ORD == RowMajor){
      dist_ = width;
    }else{
      dist_ = height;
    }
  }

  // destructor
  ~Matrix () { delete [] data_; }

  // assignment operator
  using BASE::operator=;
  Matrix & operator=(const Matrix & A2)
  {
    // matrices need to have the same dimensions!
    if (height_ != A2.height()){
      throw A2.height();
      return *this;
    }else if (width_ != A2.width()){
      throw A2.width();
      return *this;
    }

    // setting
    for (size_t i = 0; i < height_; i++){
      for (size_t j = 0; j < width_; j++){
        if (ORD == RowMajor) {
          data_[i*width_ + j] = A2(i, j);
        } else {
          data_[j*height_ + i] = A2(i, j);
        }
      }
    } 
    return *this;
  }

  // move assignment operator
  Matrix & operator= (Matrix && A2){
    std::swap(height_, A2.height_);
    std::swap(width_, A2.width_);
    std::swap(dist_, A2.dist_);
    std::swap(data_, A2.data_);

    return *this;
  }

};


template <ORDERING ORD>
MatrixView<double, ORD> Inverse (const MatrixView<double, ORD> & M) {
	/*	
		'augment'(Erweitern) the matrix (top) by the identity (=Einheitsmatrix) on the bottom
		Turn the matrix on top into the identity by elementary column ops
		The matrix on the bottom is the inverse (this was the identity matrix)
			Elementary column ops (=Reienopperator): Swap 2 columns, multiply a column by a scalar & add 2 columns
	*/

  // pivot element algorithm with pivot element at (i, j)
  auto pivot = [](MatrixView<double, ORD> C, size_t i, size_t j, size_t n){
    auto pivcol = C.Col(j); 
    
    C.swapcols(i, j);

    C.Col(i) = (1/C(n + i, i))*pivcol;  

    for (size_t k = 0; k < n; k++){
      if (k != i){
        auto col = C.Col(k);

        double C_i_k = C(n + i, k);

        col = col + (-C_i_k)*pivcol;
      } 
    }
  };
    
  //if the matrix isn't square: exit (error)
  if(M.width() != M.height()){
    throw M.width();
  }

  size_t n = M.height();

  // create a 2nxn matrix (C) to work with
  Matrix<double, ORD> C (2*n, n);

  // create an identity matrix inside
  for (size_t i=0; i < n; i++){
    for (size_t j=i; j < n; j++){
      if (i == j){
        C(i, j) = 1;
      }
      else{
        C(i, j) = 0;
      }
    }
  }

  C.Rows(n, n).deepcopy(M);

  // Perform elementary column operations
  // i is the pivot row
  for (size_t i=0; i < n; i++){
    for (size_t j=i; j < n; j++){
      if (C(n + i, j) != 0){
        pivot(C, i, j, n);
        break;
      }
      else if (j == (n-1)) {
        throw 0;
      }
    }
  }
  return C.Rows(0, n);
}


// output stream operator (without variadic templates)
template <typename T, ORDERING ORD>
std::ostream & operator<< (std::ostream & ost, const MatrixView<T, ORD> & A){
  if ((A.width() > 0) && (A.height() > 0)){
    for (size_t i = 0; i < A.height(); i++){
      ost << "(";
      for (size_t j = 0; j < A.width(); j++){
        // matrix elements are separated by tabs
        ost << A(i, j) << "\t";
      }
      ost << ")" << std::endl;
    }
  }else{
    ost << "empty matrix";
  }
  ost << std::endl;
  return ost;
}

}  // namespace ASC_bla

#endif
