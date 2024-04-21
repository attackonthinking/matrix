#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <numeric>

template <class T>
class matrix {
  template <typename T_it>
  struct basic_col_iterator {
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = T_it&;
    using pointer = T_it*;
    using iterator_category = std::random_access_iterator_tag;

    basic_col_iterator() = default;

    basic_col_iterator(pointer current, size_t col, size_t cols) : _current(current), _col(col), _cols(cols) {}

    operator basic_col_iterator<const T_it>() const {
      return {_current, _col, _cols};
    }

    reference operator*() const {
      return _current[_col];
    }

    reference operator[](difference_type rhs) const {
      return *(*this + rhs);
    }

    pointer operator->() const {
      return _current + _col;
    }

    basic_col_iterator& operator++() {
      _current += _cols;
      return *this;
    }

    basic_col_iterator operator++(int) {
      basic_col_iterator tmp = *this;
      ++*this;
      return tmp;
    }

    basic_col_iterator& operator--() {
      _current -= _cols;
      return *this;
    }

    basic_col_iterator operator--(int) {
      basic_col_iterator tmp = *this;
      --*this;
      return tmp;
    }

    basic_col_iterator& operator+=(difference_type diff) {
      _current += diff * static_cast<difference_type>(_cols);
      return *this;
    }

    basic_col_iterator& operator-=(difference_type diff) {
      _current -= diff * static_cast<difference_type>(_cols);
      return *this;
    }

    friend basic_col_iterator operator+(const basic_col_iterator& lhs, difference_type diff) {
      return (basic_col_iterator(lhs) += diff);
    }

    friend basic_col_iterator operator-(const basic_col_iterator& lhs, difference_type diff) {
      return (basic_col_iterator(lhs) -= diff);
    }

    friend basic_col_iterator operator+(difference_type diff, const basic_col_iterator& rhs) {
      return rhs + diff;
    }

    friend difference_type operator-(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return (lhs._current - rhs._current) / static_cast<difference_type>(rhs._cols);
    }

    friend bool operator==(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return lhs._current == rhs._current && lhs._col == rhs._col;
    }

    friend bool operator!=(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return !(lhs == rhs);
    }

    friend bool operator>(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return lhs._current > rhs._current;
    }

    friend bool operator<(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return lhs._current < rhs._current;
    }

    friend bool operator>=(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return !(lhs < rhs);
    }

    friend bool operator<=(const basic_col_iterator& lhs, const basic_col_iterator& rhs) {
      return !(lhs > rhs);
    }

  private:
    pointer _current;
    size_t _col;
    size_t _cols;
  };

public:
  using value_type = T;

  using reference = T&;
  using const_reference = const T&;

  using pointer = T*;
  using const_pointer = const T*;

  using iterator = pointer;
  using const_iterator = const_pointer;

  using row_iterator = pointer;
  using const_row_iterator = const_pointer;

  using col_iterator = basic_col_iterator<value_type>;
  using const_col_iterator = basic_col_iterator<const value_type>;

public:
  matrix() : _rows(0), _cols(0), _data(nullptr) {}

  matrix(size_t rows, size_t cols)
      : _rows(rows * cols == 0 ? 0 : rows),
        _cols(rows * cols == 0 ? 0 : cols),
        _data(rows * cols == 0 ? nullptr : new value_type[rows * cols]{}) {}

  template <size_t Rows, size_t Cols>
  matrix(const T (&init)[Rows][Cols]) : _rows(Rows),
                                        _cols(Cols),
                                        _data(new value_type[_rows * _cols]) {
    for (size_t row = 0; row < _rows; row++) {
      for (size_t col = 0; col < _cols; col++) {
        _data[_cols * row + col] = init[row][col];
      }
    }
  }

  matrix(const matrix& other)
      : _rows(other._rows),
        _cols(other._cols),
        _data(_rows * _cols == 0 ? nullptr : new value_type[_rows * _cols]) {
    if (_data == nullptr) {
      return;
    }
    std::copy(other.begin(), other.end(), begin());
  }

  matrix& operator=(const matrix& other) {
    if (&other == this) {
      return *this;
    }
    matrix(other).swap(*this);
    return *this;
  }

  ~matrix() {
    delete[] _data;
  }

  void swap(matrix& other) {
    std::swap(_rows, other._rows);
    std::swap(_cols, other._cols);
    std::swap(_data, other._data);
  }

  // Iterators

  iterator begin() {
    return _data;
  }

  const_iterator begin() const {
    return _data;
  }

  iterator end() {
    return begin() + size();
  }

  const_iterator end() const {
    return begin() + size();
  }

  row_iterator row_begin(size_t row) {
    assert(row < _rows);
    return begin() + row * _cols;
  }

  const_row_iterator row_begin(size_t row) const {
    assert(row < _rows);
    return begin() + row * _cols;
  }

  row_iterator row_end(size_t row) {
    return row_begin(row) + _cols;
  }

  const_row_iterator row_end(size_t row) const {
    return row_begin(row) + _cols;
  }

  col_iterator col_begin(size_t col) {
    assert(col < _cols);
    return {begin(), col, _cols};
  }

  const_col_iterator col_begin(size_t col) const {
    assert(col < _cols);
    return {begin(), col, _cols};
  }

  col_iterator col_end(size_t col) {
    return col_begin(col) + rows();
  }

  const_col_iterator col_end(size_t col) const {
    return col_begin(col) + rows();
  }

  // Size

  size_t rows() const {
    return _rows;
  }

  size_t cols() const {
    return _cols;
  }

  size_t size() const {
    return _rows * _cols;
  }

  bool empty() const {
    return size() == 0;
  }

  // Elements access

  reference operator()(size_t row, size_t col) {
    return _data[_cols * row + col];
  }

  const_reference operator()(size_t row, size_t col) const {
    return _data[_cols * row + col];
  }

  pointer data() {
    return _data;
  }

  const_pointer data() const {
    return _data;
  }

  // Comparison

  friend bool operator==(const matrix& left, const matrix& right) {
    if (left.rows() == right._rows && left.cols() == right.cols()) {
      return std::equal(left.begin(), left.end(), right.begin());
    }
    return false;
  }

  friend bool operator!=(const matrix& left, const matrix& right) {
    return !(right == left);
  }

  // Arithmetic operations

  matrix& operator+=(const matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::plus<>{});
    return *this;
  }

  matrix& operator-=(const matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::minus<>{});
    return *this;
  }

  matrix& operator*=(const_reference factor) {
    std::transform(begin(), end(), begin(), [&factor](value_type elem) { return elem * factor; });
    return *this;
  }

  matrix& operator*=(const matrix& other) {
    (*this * other).swap(*this);
    return *this;
  }

  friend matrix operator+(const matrix& left, const matrix& right) {
    matrix result = left;
    result += right;
    return result;
  }

  friend matrix operator-(const matrix& left, const matrix& right) {
    matrix result = left;
    result -= right;
    return result;
  }

  friend matrix operator*(const matrix& left, const_reference right) {
    matrix result = left;
    result *= right;
    return result;
  }

  friend matrix operator*(const_reference left, const matrix& right) {
    return right * left;
  }

  friend matrix operator*(const matrix& left, const matrix& right) {
    matrix result(left.rows(), right.cols());
    matrix::iterator it = result.begin();
    for (size_t r = 0; r < left.rows(); r++) {
      for (size_t c = 0; c < right.cols(); c++) {
        *it = std::inner_product(left.row_begin(r), left.row_end(r), right.col_begin(c), value_type(0));
        it++;
      }
    }
    return result;
  }

private:
  size_t _rows;
  size_t _cols;
  pointer _data;
};
