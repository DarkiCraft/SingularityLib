#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>  // for size_t

#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Sglty {

template <typename T, size_t RowsAtCompileTime, size_t ColsAtCompileTime>
class Matrix;

template <typename T, size_t DimsAtCompileTime>
using ColVector = Matrix<T, DimsAtCompileTime, 1>;

template <typename T, size_t DimsAtCompileTime>
using RowVector = Matrix<T, 1, DimsAtCompileTime>;

template <typename T, size_t RowsAtCompileTime, size_t ColsAtCompileTime>
class Matrix {
  static_assert(std::is_arithmetic_v<T>,
                "Sglty::Matrix: Only arithmetic types allowed");
  static_assert(
      RowsAtCompileTime > 0 && ColsAtCompileTime > 0,
      "Sglty::Matrix: RowsAtCompileTime and ColsAtCompileTime must be > 0");

 public:
  Matrix() = default;

  Matrix(const std::initializer_list<T>& list)
    requires(RowsAtCompileTime == 1 || ColsAtCompileTime == 1)
  {
    if (list.size() != RowsAtCompileTime * ColsAtCompileTime) {
      throw std::invalid_argument(
          "Sglty::Matrix: Number of elements MUST equal DimsAtCompileTime = " +
          std::to_string(Dims()));
    }
    std::copy(list.begin(), list.end(), data_.begin());
  }

  Matrix(const std::initializer_list<std::initializer_list<T>>& list)
    requires(RowsAtCompileTime > 1 && ColsAtCompileTime > 1)
  {
    if (list.size() != Rows()) {
      throw std::invalid_argument(
          "Sglty::Matrix: Number of rows MUST equal RowsAtCompileTime = " +
          std::to_string(Rows()));
    }
    size_t row_index = 0;
    for (auto& row : list) {
      if (row.size() != Cols()) {
        throw std::invalid_argument(
            "Sglty::Matrix: Number of columns MUST equal ColsAtCompileTime "
            "= " +
            std::to_string(Cols()));
      }
      size_t col_index = 0;
      for (auto& e : row) {
        this->operator()(row_index, col_index) = e;
        col_index++;
      }
      row_index++;
    }
  }

  Matrix(const Matrix& other) = default;
  Matrix(Matrix&& other) noexcept = default;

  Matrix& operator=(const Matrix& other) = default;
  Matrix& operator=(Matrix&& other) noexcept = default;

  static constexpr size_t Rows() noexcept { return RowsAtCompileTime; }
  static constexpr size_t Cols() noexcept { return ColsAtCompileTime; }
  static constexpr size_t Dims() noexcept
    requires(Rows() == 1 || Cols() == 1)
  {
    return Rows() == 1 ? Cols() : Rows();
  }

  static constexpr bool IsRowVector() noexcept { return Rows() == 1; }
  static constexpr bool IsColVector() noexcept { return Cols() == 1; }

  static constexpr bool IsScalar() noexcept {
    return IsRowVector() && IsColVector();
  }

  static constexpr bool IsVector() noexcept {
    return IsRowVector() || IsColVector();
  }

  ColVector<T, Rows()> GetColVector(const size_t col) const
  // requires(Cols() > 0)
  {
    ColVector<T, Rows()> result;
    for (size_t i = 0; i < Rows(); i++) {
      result(i) = this->operator()(i, col);
    }
    return result;
  }

  RowVector<T, Cols()> GetRowVector(const size_t row) const
  // requires(Rows() > 0)
  {
    RowVector<T, Cols()> result;
    for (size_t i = 0; i < Cols(); i++) {
      result(i) = this->operator()(row, i);
    }
    return result;
  }

  static constexpr Matrix Identity() noexcept
    requires(Rows() == Cols())
  {
    Matrix result;
    for (size_t i = 0; i < Rows(); i++) {
      result(i, i) = 1;
    }
    return result;
  }

  template <typename Self>
  auto& operator()(this Self&& self, const size_t index)
    requires(IsVector())
  {
    return self.data_[index];
  }

  template <typename Self>
  auto& operator()(this Self&& self, const size_t row, const size_t col)
  // requires(!IsVector())
  {
    return self.data_[row + col * Rows()];
  }

  operator T() const
    requires(IsScalar())
  {
    return data_.front();
  }

  auto operator+=(const Matrix& other) {
    for (size_t i = 0; i < Rows(); i++) {
      for (size_t j = 0; j < Cols(); j++) {
        this->operator()(i, j) += other(i, j);
      }
    }
    return *this;
  }

  auto operator+(const Matrix& other) const {
    auto result(*this);
    return result += other;
  }

  auto operator*=(const T scalar) {
    for (size_t i = 0; i < data_.size(); i++) {
      data_[i] *= scalar;
    }

    return *this;
  }

  auto operator*(const T scalar) const {
    auto result(*this);
    return result *= scalar;
  }

  T operator*(const ColVector<T, Cols()>& other) const
    requires(IsRowVector())
  {
    T result = 0;
    for (size_t i = 0; i < Cols(); i++) {
      result += this->operator()(i) * other(i);
    }
    return result;
  }

  template <size_t OtherCols>
  auto operator*(const Matrix<T, Cols(), OtherCols>& other) const {
    Matrix<T, Rows(), other.Cols()> result;
    for (size_t i = 0; i < result.Rows(); i++) {
      for (size_t j = 0; j < result.Cols(); j++) {
        result(i, j) = this->GetRowVector(i) * other.GetColVector(j);
      }
    }
    return result;
  }

  void Print() const {
    for (size_t i = 0; i < Rows(); i++) {
      for (size_t j = 0; j < Cols(); j++) {
        std::cout << this->operator()(i, j) << " ";
      }
      std::cout << '\n';
    }
  }

 private:
  std::array<T, Rows() * Cols()> data_{};
};

}  // namespace Sglty

#endif  // MATRIX_HPP