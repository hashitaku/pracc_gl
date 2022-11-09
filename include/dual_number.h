/**
 * @file dual_number.hpp
 * @brief constexpr dual_number class
 * @author hashitaku
 * @data 2019/7/27
 */

#ifndef SEMI_CPP_DUAL_NUM
#define SEMI_CPP_DUAL_NUM

#include <ostream>

/**
 * @class dual_num
 * @brief constexpr dual_number class
 */
template <typename Tp>
class dual_num {
private:
    Tp re_;
    Tp im_;

public:
    using value_type = Tp;

    constexpr dual_num(Tp a = value_type{}, Tp b = value_type{}) : re_{a}, im_{b} {}

    constexpr dual_num(const dual_num<Tp>& x) : re_{x.real()}, im_{x.imag()} {}

    template <typename U>
    constexpr dual_num(const dual_num<U>& x) : re_{x.real()}, im_{x.imag()} {}

    constexpr Tp real() const { return re_; }
    constexpr Tp imag() const { return im_; }

    template <typename U>
    constexpr dual_num<Tp>& operator=(const U& a) {
        re_ = a;
        im_ = Tp();
        return *this;
    }

    template <typename U>
    constexpr dual_num<Tp>& operator+=(const U& a) {
        re_ += a;
        return *this;
    }

    template <typename U>
    constexpr dual_num<Tp>& operator-=(const U& a) {
        re_ -= a;
        return *this;
    }

    template <typename U>
    constexpr dual_num<Tp>& operator*=(const U& a) {
        re_ *= a;
        im_ *= a;
        return *this;
    }

    template <typename U>
    constexpr dual_num<Tp>& operator/=(const U& a) {
        re_ /= a;
        im_ /= a;
        return *this;
    }

    constexpr dual_num<Tp>& operator=(const dual_num<Tp>& z) {
        re_ = z.real();
        im_ = z.imag();
        return *this;
    }

    constexpr dual_num<Tp>& operator+=(const dual_num<Tp>& z) {
        re_ += z.real();
        im_ += z.imag();
        return *this;
    }

    constexpr dual_num<Tp>& operator-=(const dual_num<Tp>& z) {
        re_ -= z.real();
        im_ -= z.imag();
        return *this;
    }

    constexpr dual_num<Tp>& operator*=(const dual_num<Tp>& z) {
        const Tp r = re_ * z.real();
        im_ = re_ * z.imag() + im_ * z.real();
        re_ = r;
        return *this;
    }

    constexpr dual_num<Tp>& operator/=(const dual_num<Tp>& z) {
        const Tp r = re_ * z.real();
        const Tp n = z.real() * z.real();
        im_ = (im_ * z.real() - re_ * z.imag()) / n;
        re_ = r / n;
        return *this;
    }
};

template <typename Tp>
constexpr Tp real(const dual_num<Tp>& z) {
    return z.real();
}

template <typename Tp>
constexpr Tp imag(const dual_num<Tp>& z) {
    return z.imag();
}

template <typename Tp>
constexpr dual_num<Tp> operator+(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    dual_num<Tp> retval = x;
    retval += y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator+(const dual_num<T>& x, const U& y) {
    dual_num<T> retval = x;
    retval += y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator+(const U& x, const dual_num<T>& y) {
    dual_num<T> retval = y;
    retval += x;
    return retval;
}

template <typename Tp>
constexpr dual_num<Tp> operator-(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    dual_num<Tp> retval = x;
    retval -= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator-(const dual_num<T>& x, const U& y) {
    dual_num<T> retval = x;
    retval -= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator-(const U& x, const dual_num<T>& y) {
    dual_num<T> retval(x, -y.imag());
    retval -= y.real();
    return retval;
}

template <typename Tp>
constexpr dual_num<Tp> operator*(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    dual_num<Tp> retval = x;
    retval *= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator*(const dual_num<T>& x, const U& y) {
    dual_num<T> retval = x;
    retval *= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator*(const U& x, const dual_num<T>& y) {
    dual_num<T> retval = y;
    retval *= x;
    return retval;
}

template <typename Tp>
constexpr dual_num<Tp> operator/(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    dual_num<Tp> retval = x;
    retval /= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator/(const dual_num<T>& x, const U& y) {
    dual_num<T> retval = x;
    retval /= y;
    return retval;
}

template <typename T, typename U>
constexpr dual_num<T> operator/(const U& x, const dual_num<T>& y) {
    dual_num<T> retval = x;
    retval /= y;
    return retval;
}

template <typename Tp>
constexpr dual_num<Tp> operator+(const dual_num<Tp>& x) {
    return x;
}

template <typename Tp>
constexpr dual_num<Tp> operator-(const dual_num<Tp>& x) {
    return {-x.real(), -x.imag()};
}

template <typename Tp>
constexpr bool operator==(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    return x.real() == y.real() && x.imag() == y.imag();
}

template <typename Tp>
constexpr bool operator==(const dual_num<Tp>& x, const Tp& y) {
    return x.real() == y && x.imag() == Tp();
}

template <typename Tp>
constexpr bool operator==(const Tp& x, const dual_num<Tp>& y) {
    return x == y.real() && Tp() == y.imag();
}

template <typename Tp>
constexpr bool operator!=(const dual_num<Tp>& x, const dual_num<Tp>& y) {
    return x.real() != y.real() || x.imag() != y.imag();
}

template <typename Tp>
constexpr bool operator!=(const dual_num<Tp>& x, const Tp& y) {
    return x.real() != y || x.imag() != Tp();
}

template <typename Tp>
constexpr bool operator!=(const Tp& x, const dual_num<Tp>& y) {
    return x != y.real() || Tp() != y.imag();
}

template <typename Tp, typename Char, typename Traits>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const dual_num<Tp>& x) {
    os << '(' << x.real() << ',' << x.imag() << ')';
    return os;
}

#endif  // SEMI_CPP_DUAL_NUM
