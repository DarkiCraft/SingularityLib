#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <cstddef>  // for size_t
#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>

namespace Sglty {

/**
 * @brief Forward declaration of the Matrix class template.
 *
 * @tparam T The type of the elements in the matrix.
 * @tparam RowsAtCompileTime The number of rows in the matrix at compile time.
 * @tparam ColsAtCompileTime The number of columns in the matrix at compile
 * time.
 */
template <typename T, size_t RowsAtCompileTime, size_t ColsAtCompileTime>
class Matrix;

/**
 * @brief Type alias for a column vector using the Matrix class.
 *
 * @tparam T The type of the elements in the column vector.
 * @tparam DimsAtCompileTime The number of rows in the column vector at compile
 * time.
 */
template <typename T, size_t DimsAtCompileTime>
using ColVector = Matrix<T, DimsAtCompileTime, 1>;

/**
 * @brief Type alias for a row vector using the Matrix class.
 *
 * @tparam T The type of the elements in the row vector.
 * @tparam DimsAtCompileTime The number of columns in the row vector at compile
 * time.
 */
template <typename T, size_t DimsAtCompileTime>
using RowVector = Matrix<T, 1, DimsAtCompileTime>;

/**
 * @brief A class representing a mathematical matrix.
 *
 * This class supports basic matrix operations such as addition, scalar
 * multiplication, and matrix multiplication. It also provides functionality to
 * create row and column vectors, as well as an identity matrix.
 *
 * @tparam T The type of the elements in the matrix (must be an arithmetic
 * type).
 * @tparam RowsAtCompileTime The number of rows in the matrix at compile time
 * (must be > 0).
 * @tparam ColsAtCompileTime The number of columns in the matrix at compile time
 * (must be > 0).
 */
template <typename T, size_t RowsAtCompileTime, size_t ColsAtCompileTime>
class Matrix {
  static_assert(std::is_arithmetic_v<T>,
                "Sglty::Matrix: Only arithmetic types allowed");
  static_assert(
      RowsAtCompileTime > 0 && ColsAtCompileTime > 0,
      "Sglty::Matrix: RowsAtCompileTime and ColsAtCompileTime must be > 0");

 public:
  /// @brief Default constructor for the Matrix class.
  Matrix() = default;

  /**
   * @brief Constructor that initializes a vector from an initializer list.
   *
   * This constructor is enabled only for row or column vectors.
   *
   * @param list An initializer list containing the elements of the vector.
   */
  Matrix(const std::initializer_list<T>& list)
    requires(RowsAtCompileTime == 1 || ColsAtCompileTime == 1)
  {
    std::copy(list.begin(), list.end(), data_.begin());
  }

  /**
   * @brief Constructor that initializes a matrix from a 2D initializer list.
   *
   * This constructor is enabled only for matrices with more than one row and
   * column.
   *
   * @param list A 2D initializer list containing the elements of the matrix.
   * @throws std::invalid_argument if the number of rows or columns does not
   * match the specified dimensions.
   */
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

  /// @brief Copy constructor for the Matrix class.
  Matrix(const Matrix& other) = default;

  /// @brief Move constructor for the Matrix class.
  Matrix(Matrix&& other) noexcept = default;

  /// @brief Copy assignment operator for the Matrix class.
  Matrix& operator=(const Matrix& other) = default;

  /// @brief Move assignment operator for the Matrix class.
  Matrix& operator=(Matrix&& other) noexcept = default;

  /**
   * @brief Returns the number of rows in the matrix at compile time.
   *
   * @return The number of rows in the matrix.
   */
  static constexpr size_t Rows() noexcept { return RowsAtCompileTime; }

  /**
   * @brief Returns the number of columns in the matrix at compile time.
   *
   * @return The number of columns in the matrix.
   */
  static constexpr size_t Cols() noexcept { return ColsAtCompileTime; }

  /**
   * @brief Returns the number of dimensions for vectors.
   *
   * This function is enabled only for row or column vectors.
   *
   * @return The number of dimensions in the vector.
   */
  static constexpr size_t Dims() noexcept
    requires(Rows() == 1 || Cols() == 1)
  {
    return Rows() == 1 ? Cols() : Rows();
  }

  /**
   * @brief Checks if the matrix is a row vector.
   *
   * @return True if the matrix is a row vector, false otherwise.
   */
  static constexpr bool IsRowVector() noexcept { return Rows() == 1; }

  /**
   * @brief Checks if the matrix is a column vector.
   *
   * @return True if the matrix is a column vector, false otherwise.
   */
  static constexpr bool IsColVector() noexcept { return Cols() == 1; }

  /**
   * @brief Checks if the matrix is a scalar (1x1 matrix).
   *
   * @return True if the matrix is a scalar, false otherwise.
   */
  static constexpr bool IsScalar() noexcept {
    return IsRowVector() && IsColVector();
  }

  /**
   * @brief Checks if the matrix is a vector (either row or column).
   *
   * @return True if the matrix is a vector, false otherwise.
   */
  static constexpr bool IsVector() noexcept {
    return IsRowVector() || IsColVector();
  }

  /**
   * @brief Retrieves a column vector from the matrix.
   *
   * @param col The index of the column to retrieve.
   * @return A column vector containing the elements of the specified column.
   */
  ColVector<T, Rows()> GetColVector(const size_t col) const
  // requires(Cols() > 0)
  {
    ColVector<T, Rows()> result;
    for (size_t i = 0; i < Rows(); i++) {
      result(i) = this->operator()(i, col);
    }
    return result;
  }

  /**
   * @brief Retrieves a row vector from the matrix.
   *
   * @param row The index of the row to retrieve.
   * @return A row vector containing the elements of the specified row.
   */
  RowVector<T, Cols()> GetRowVector(const size_t row) const
  // requires(Rows() > 0)
  {
    RowVector<T, Cols()> result;
    for (size_t i = 0; i < Cols(); i++) {
      result(i) = this->operator()(row, i);
    }
    return result;
  }

  /**
   * @brief Creates an identity matrix of the same size.
   *
   * This function is enabled only for square matrices.
   *
   * @return An identity matrix of the same size.
   */
  static constexpr Matrix Identity() noexcept
    requires(Rows() == Cols())
  {
    Matrix result;
    for (size_t i = 0; i < Rows(); i++) {
      result(i, i) = 1;
    }
    return result;
  }

  /**
   * @brief Accesses an element in the matrix or vector.
   *
   * @param self A reference to the current instance.
   * @param index The index of the element to access.
   * @return A reference to the specified element.
   */
  template <typename Self>
  auto& operator()(this Self&& self, const size_t index)
    requires(IsVector())
  {
    return self.data_[index];
  }

  /**
   * @brief Accesses an element in the matrix.
   *
   * @param self A reference to the current instance.
   * @param row The row index of the element to access.
   * @param col The column index of the element to access.
   * @return A reference to the specified element.
   */
  template <typename Self>
  auto& operator()(this Self&& self, const size_t row, const size_t col)
  // requires(!IsVector())
  {
    return self.data_[row + col * Rows()];
  }

  /**
   * @brief Converts the matrix to a scalar value.
   *
   * This function is enabled only for scalar matrices.
   *
   * @return The scalar value contained in the matrix.
   */
  operator T() const
    requires(IsScalar())
  {
    return data_.front();
  }

  /**
   * @brief Adds another matrix to this matrix.
   *
   * @param other The matrix to add.
   * @return A reference to this matrix after addition.
   */
  auto operator+=(const Matrix& other) {
    for (size_t i = 0; i < Rows(); i++) {
      for (size_t j = 0; j < Cols(); j++) {
        this->operator()(i, j) += other(i, j);
      }
    }
    return *this;
  }

  /**
   * @brief Adds another matrix to this matrix and returns the result.
   *
   * @param other The matrix to add.
   * @return A new matrix that is the sum of this matrix and the other matrix.
   */
  auto operator+(const Matrix& other) const {
    auto result(*this);
    return result += other;
  }

  /**
   * @brief Multiplies this matrix by a scalar.
   *
   * @param scalar The scalar to multiply by.
   * @return A reference to this matrix after multiplication.
   */
  auto operator*=(const T scalar) {
    for (size_t i = 0; i < data_.size(); i++) {
      data_[i] *= scalar;
    }
    return *this;
  }

  /**
   * @brief Multiplies this matrix by a scalar and returns the result.
   *
   * @param scalar The scalar to multiply by.
   * @return A new matrix that is the product of this matrix and the scalar.
   */
  auto operator*(const T scalar) const {
    auto result(*this);
    return result *= scalar;
  }

  /**
   * @brief Multiplies this row vector by a column vector.
   *
   * @param other The column vector to multiply by.
   * @return The dot product of the row vector and the column vector.
   */
  T operator*(const ColVector<T, Cols()>& other) const
    requires(IsRowVector())
  {
    T result = 0;
    for (size_t i = 0; i < Cols(); i++) {
      result += this->operator()(i) * other(i);
    }
    return result;
  }

  /**
   * @brief Multiplies this matrix by another matrix.
   *
   * @tparam OtherCols The number of columns in the other matrix.
   * @param other The matrix to multiply by.
   * @return A new matrix that is the product of this matrix and the other
   * matrix.
   */
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

  /**
   * @brief Prints the matrix to the standard output.
   */
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