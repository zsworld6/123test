#include "int2048.h"
#include <string>

namespace sjtu {

// ============================================================================
// Constructors and Destructor
// ============================================================================

int2048::int2048() : vals_(1, 0), sign_bit_(1) {}

int2048::int2048(long long x) {
    if (x == 0) {
        vals_ = std::vector<int>(1, 0);
        sign_bit_ = 1;
        return;
    }
    
    vals_.clear();
    sign_bit_ = (x > 0) ? 1 : -1;
    if (x < 0) {
        x = -x;
    }
    
    // Reserve space for efficiency
    vals_.reserve(static_cast<int>(std::log10(x) / kLEN) + 1);
    while (x > 0) {
        vals_.emplace_back(x % kBASE);
        x /= kBASE;
    }
}

int2048::int2048(const std::string &s) {
    read(s);
}

int2048::int2048(const int2048 &x) : vals_(x.vals_), sign_bit_(x.sign_bit_) {}

int2048::int2048(const poly &a) {
    *this = to_int2048(a);
}

int2048::~int2048() {
    vals_.clear();
    vals_.shrink_to_fit();
}

// ============================================================================
// Type Conversion Operators
// ============================================================================

int2048::operator double() const {
    double result = 0.0;
    for (int i = len() - 1; i >= 0; --i) {
        result *= kBASE;
        result += vals_[i];
    }
    return result * sign_bit_;
}

int2048::operator std::string() const {
    // Handle zero case
    if (*this == int2048(0)) {
        return "0";
    }
    
    std::string s;
    if (sign_bit_ == -1) {
        s = "-";
    }
    
    // Add the most significant digit without leading zeros
    s += std::to_string(vals_.back());
    
    // Add remaining digits with leading zeros
    for (int i = len() - 2; i >= 0; --i) {
        std::string digit = std::to_string(vals_[i]);
        s += std::string(kLEN - digit.size(), '0') + digit;
    }
    
    return s;
}

// ============================================================================
// Input/Output Functions
// ============================================================================

void int2048::read(const std::string &s) {
    int start_pos = 0;
    
    // Handle sign
    if (!s.empty() && s[0] == '-') {
        sign_bit_ = -1;
        start_pos = 1;
    } else {
        sign_bit_ = 1;
    }
    
    vals_.clear();
    vals_.reserve((s.size() - start_pos) / kLEN + 1);
    
    // Read digits from right to left in chunks of kLEN
    for (int i = static_cast<int>(s.size()) - 1; i >= start_pos; i -= kLEN) {
        int chunk = 0;
        int chunk_len = std::min(kLEN, i + 1 - start_pos);
        for (int j = chunk_len - 1; j >= 0; --j) {
            chunk = chunk * 10 + (s[i - j] - '0');
        }
        vals_.emplace_back(chunk);
    }
    
    // Remove leading zeros
    while (vals_.size() > 1 && vals_.back() == 0) {
        vals_.pop_back();
    }
}

void int2048::print() {
    std::cout << (*this);
}

// ============================================================================
// Addition and Subtraction Helper Functions
// ============================================================================

int2048 add(int2048, const int2048 &);
int2048 minus(int2048, const int2048 &);

/**
 * Add two big integers (handles both positive and negative numbers)
 * @param a First operand (passed by value for modification)
 * @param b Second operand (const reference)
 * @return Sum of a and b
 */
int2048 add(int2048 a, const int2048 &b) {
    // Handle negative numbers by converting to subtraction
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return -add(-a, -b);
    } else if (b.sign_bit_ == -1) {
        return minus(a, -b);
    } else if (a.sign_bit_ == -1) {
        return minus(b, -a);
    }
    
    // Both numbers are positive
    int maxlen = std::max(a.vals_.size(), b.vals_.size());
    a.vals_.resize(maxlen + 1);
    
    bool carry = false;
    for (int i = 0; i < static_cast<int>(b.vals_.size()) || carry; ++i) {
        if (i < static_cast<int>(b.vals_.size())) {
            a.vals_[i] += b.vals_[i];
        }
        
        if (a.vals_[i] >= kBASE) {
            a.vals_[i + 1]++;
            a.vals_[i] -= kBASE;
            carry = true;
        } else {
            carry = false;
        }
    }
    
    // Remove leading zeros
    while (a.vals_.size() > 1 && a.vals_.back() == 0) {
        a.vals_.pop_back();
    }
    
    return a;
}

/**
 * Subtract two big integers (handles both positive and negative numbers)
 * @param a First operand (passed by value for modification)
 * @param b Second operand (const reference)
 * @return Difference of a and b
 */
int2048 minus(int2048 a, const int2048 &b) {
    // Handle negative numbers by converting to addition
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return minus(-b, -a);
    } else if (a.sign_bit_ == -1) {
        return -add(-a, b);
    } else if (b.sign_bit_ == -1) {
        return add(a, -b);
    }
    
    // Ensure a >= b for positive subtraction
    if (a < b) {
        return -minus(b, a);
    }
    
    a.vals_.resize(a.vals_.size() + 1);
    
    bool borrow = false;
    for (int i = 0; i < static_cast<int>(b.vals_.size()) || borrow; ++i) {
        if (i < static_cast<int>(b.vals_.size())) {
            a.vals_[i] -= b.vals_[i];
        }
        
        if (a.vals_[i] < 0) {
            a.vals_[i] += kBASE;
            a.vals_[i + 1]--;
            borrow = true;
        } else {
            borrow = false;
        }
    }
    
    // Remove leading zeros
    while (a.vals_.size() > 1 && a.vals_.back() == 0) {
        a.vals_.pop_back();
    }
    
    return a;
}

// ============================================================================
// Member Function Implementations for Addition/Subtraction
// ============================================================================

int2048 &int2048::add(const int2048 &b) {
    *this = sjtu::add(*this, b);
    return *this;
}

int2048 &int2048::minus(const int2048 &b) {
    *this = sjtu::minus(*this, b);
    return *this;
}

// ============================================================================
// Unary Operators
// ============================================================================

int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 result = *this;
    result.sign_bit_ = -result.sign_bit_;
    return result;
}

// ============================================================================
// Assignment and Arithmetic Operators
// ============================================================================

int2048 &int2048::operator=(const int2048 &b) {
    if (this != &b) {
        vals_ = b.vals_;
        sign_bit_ = b.sign_bit_;
    }
    return *this;
}

int2048 operator+(int2048 a, const int2048 &b) {
    return add(a, b);
}

int2048 &int2048::operator+=(const int2048 &b) {
    return add(b);
}

int2048 operator-(int2048 a, const int2048 &b) {
    return minus(a, b);
}

int2048 &int2048::operator-=(const int2048 &b) {
    return minus(b);
}

// ============================================================================
// Stream Operators
// ============================================================================

std::istream &operator>>(std::istream &in, int2048 &a) {
    std::string s;
    in >> s;
    a = int2048(s);
    return in;
}

/**
 * Output operator for int2048
 * WARNING: This implementation depends on kBASE and kLEN constants!
 * kBASE must be 1000 and kLEN must be 3 for proper zero-padding
 */
std::ostream &operator<<(std::ostream &out, const int2048 &a) {
    // Print negative sign for non-zero negative numbers
    if (a.sign_bit_ == -1 && (a.vals_.size() > 1 || a.vals_[0] != 0)) {
        out << '-';
    }
    
    // Print digits from most significant to least significant
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; --i) {
        if (i == static_cast<int>(a.vals_.size()) - 1) {
            // Most significant digit: no leading zeros
            out << a.vals_[i];
        } else {
            // Other digits: pad with zeros to kLEN digits
            if (a.vals_[i] >= 100) {
                out << a.vals_[i];
            } else if (a.vals_[i] >= 10) {
                out << '0' << a.vals_[i];
            } else {
                out << "00" << a.vals_[i];
            }
        }
    }
    
    return out;
}

// ============================================================================
// Comparison Operators
// ============================================================================

bool operator==(const int2048 &a, const int2048 &b) {
    // Both are zero
    if (a.vals_.size() == 1 && a.vals_[0] == 0 && 
        b.vals_.size() == 1 && b.vals_[0] == 0) {
        return true;
    }
    
    // Different signs
    if (a.sign_bit_ != b.sign_bit_) {
        return false;
    }
    
    // Different lengths
    if (a.vals_.size() != b.vals_.size()) {
        return false;
    }
    
    // Compare digit by digit
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; --i) {
        if (a.vals_[i] != b.vals_[i]) {
            return false;
        }
    }
    
    return true;
}

bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
}

bool operator<=(const int2048 &a, const int2048 &b) {
    // Both are zero
    if (a == int2048(0) && b == int2048(0)) {
        return true;
    }
    
    // Both negative: reverse comparison of absolute values
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return (-a) >= (-b);
    }
    
    // Different signs: negative < positive
    if (a.sign_bit_ != b.sign_bit_) {
        return a.sign_bit_ < b.sign_bit_;
    }
    
    // Same sign, different lengths
    if (a.vals_.size() != b.vals_.size()) {
        return a.vals_.size() < b.vals_.size();
    }
    
    // Same sign and length: compare digit by digit
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; --i) {
        if (a.vals_[i] > b.vals_[i]) {
            return false;
        } else if (a.vals_[i] < b.vals_[i]) {
            return true;
        }
    }
    
    return true;
}

bool operator>=(const int2048 &a, const int2048 &b) {
    // Both are zero
    if (a == int2048(0) && b == int2048(0)) {
        return true;
    }
    
    // Both negative: reverse comparison of absolute values
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return (-a) <= (-b);
    }
    
    // Different signs: positive > negative
    if (a.sign_bit_ != b.sign_bit_) {
        return a.sign_bit_ > b.sign_bit_;
    }
    
    // Same sign, different lengths
    if (a.vals_.size() != b.vals_.size()) {
        return a.vals_.size() > b.vals_.size();
    }
    
    // Same sign and length: compare digit by digit
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; --i) {
        if (a.vals_[i] < b.vals_[i]) {
            return false;
        } else if (a.vals_[i] > b.vals_[i]) {
            return true;
        }
    }
    
    return true;
}

bool operator<(const int2048 &a, const int2048 &b) {
    return !(a >= b);
}

bool operator>(const int2048 &a, const int2048 &b) {
    return !(a <= b);
}

// ============================================================================
// Polynomial Conversion (for FFT multiplication)
// ============================================================================

constexpr double eps = 1e-7;

poly int2048::to_poly() const {
    poly result;
    result.reserve(vals_.size());
    
    for (int x : vals_) {
        result.push_back(x);
    }
    
    // Remove trailing zeros
    while (result.size() > 1 && std::abs(result.back().real()) < eps) {
        result.pop_back();
    }
    
    return result;
}

int2048 int2048::to_int2048(const poly &a) const {
    std::vector<long long> temp(a.size() + 1);
    
    // Convert complex numbers to integers with carry handling
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        temp[i] += static_cast<long long>(a[i].imag() / 2.0 + 0.5);
        temp[i + 1] += temp[i] / kBASE;
        temp[i] %= kBASE;
    }
    
    // Remove leading zeros
    while (temp.size() > 1 && temp.back() == 0) {
        temp.pop_back();
    }
    
    // Convert to int2048
    int2048 result;
    result.vals_.clear();
    result.vals_.reserve(temp.size());
    
    for (long long x : temp) {
        result.vals_.push_back(static_cast<int>(x));
    }
    
    // Remove any remaining leading zeros
    while (result.vals_.size() > 1 && result.vals_.back() == 0) {
        result.vals_.pop_back();
    }
    
    return result;
}

// ============================================================================
// Fast Fourier Transform (FFT) for Polynomial Multiplication
// ============================================================================

namespace polymul {

// High-memory, low-time complexity version
const double kPI = acos(-1.0);

std::vector<int> rev;                // Bit-reversal permutation array
int history_max_len = 1;             // Track maximum initialized length
std::vector<Complex> pww[35][2];     // Pre-computed roots of unity

/**
 * Initialize butterfly transform for FFT
 * Pre-computes bit-reversal permutation and roots of unity
 * @param k Log2 of the transform length
 */
void init(int k) {
    int len = 1 << k;
    
    if (len > history_max_len) {
        rev.resize(len);
        
        // Pre-compute roots of unity for different levels
        for (int i = history_max_len; i < len; i <<= 1) {
            int t = static_cast<int>(std::log2(i));
            
            // Forward transform roots
            Complex wn = exp(Complex(0, kPI / i));
            pww[t][0].resize(i);
            pww[t][0][0] = Complex(1, 0);
            for (int j = 1; j < i; ++j) {
                pww[t][0][j] = pww[t][0][j - 1] * wn;
            }
            
            // Inverse transform roots
            wn = exp(Complex(0, -kPI / i));
            pww[t][1].resize(i);
            pww[t][1][0] = Complex(1, 0);
            for (int j = 1; j < i; ++j) {
                pww[t][1][j] = pww[t][1][j - 1] * wn;
            }
        }
        
        history_max_len = len;
    }
    
    // Compute bit-reversal permutation
    for (int i = 0; i < len; ++i) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (k - 1));
    }
}

/**
 * Fast Fourier Transform
 * @param a Polynomial coefficients (modified in-place)
 * @param n Length of the transform (must be a power of 2)
 * @param inv 1 for forward transform, -1 for inverse transform
 */
void FFT(poly &a, int n, int inv) {
    // Bit-reversal permutation
    for (int i = 0; i < n; ++i) {
        if (i < rev[i]) {
            std::swap(a[i], a[rev[i]]);
        }
    }
    
    // Cooley-Tukey butterfly operations
    for (int mid = 1; mid < n; mid <<= 1) {
        int t = static_cast<int>(std::log2(mid));
        for (int i = 0; i < n; i += (mid << 1)) {
            for (int j = 0; j < mid; ++j) {
                Complex x = a[i + j];
                Complex y = pww[t][(inv == 1) ? 0 : 1][j] * a[i + j + mid];
                a[i + j] = x + y;
                a[i + j + mid] = x - y;
            }
        }
    }
    
    // Normalize for inverse transform
    if (inv == -1) {
        for (int i = 0; i < n; ++i) {
            a[i] /= n;
        }
    }
}

/**
 * Multiply two polynomials using FFT
 * @param a First polynomial
 * @param b Second polynomial
 * @return Product polynomial
 */
poly mul(poly a, poly b) {
    int maxlen = std::max(a.size(), b.size());
    int bit = 1, len = 2;
    
    // Find smallest power of 2 >= 2*maxlen - 1
    while ((1 << bit) < 2 * maxlen - 1) {
        bit++;
        len <<= 1;
    }
    
    init(bit);
    a.resize(len);
    b.resize(len);
    
    // Combine a and b into complex numbers for efficiency
    for (int i = 0; i < len; ++i) {
        a[i] = Complex(a[i].real(), b[i].real());
    }
    
    FFT(a, len, 1);
    
    // Point-wise multiplication
    for (int i = 0; i < len; ++i) {
        a[i] = a[i] * a[i];
    }
    
    FFT(a, len, -1);
    
    return a;
}

}  // namespace polymul

// ============================================================================
// Multiplication Operators
// ============================================================================

int2048 operator*(int2048 a, const int2048 &b) {
    // Convert to polynomials and multiply using FFT
    auto poly_a = a.to_poly();
    auto poly_b = b.to_poly();
    auto result = int2048(polymul::mul(poly_a, poly_b));
    
    // Set sign: positive if same sign, negative if different
    result.sign_bit_ = (a.sign_bit_ == b.sign_bit_) ? 1 : -1;
    
    return result;
}

int2048 &int2048::operator*=(const int2048 &b) {
    *this = (*this) * b;
    return *this;
}

// ============================================================================
// Bit Shift Operations (Base-10 shifts)
// ============================================================================

int2048 &int2048::left_shift(const int k) {
    if (k <= 0) {
        return *this;
    }
    
    vals_.resize(vals_.size() + k);
    
    // Shift digits to the left
    for (int i = static_cast<int>(vals_.size()) - 1; i >= k; --i) {
        vals_[i] = vals_[i - k];
    }
    
    // Fill with zeros
    for (int i = 0; i < k; ++i) {
        vals_[i] = 0;
    }
    
    return *this;
}

int2048 &int2048::right_shift(const int k) {
    if (k <= 0) {
        return *this;
    }
    
    if (k >= static_cast<int>(vals_.size())) {
        return (*this = 0);
    }
    
    // Shift digits to the right
    for (int i = k; i < static_cast<int>(vals_.size()); ++i) {
        vals_[i - k] = vals_[i];
    }
    
    // Remove shifted digits
    for (int i = 0; i < k; ++i) {
        vals_.pop_back();
    }
    
    return *this;
}

// ============================================================================
// Division Helper Functions
// ============================================================================

/**
 * Brute force division using binary search/doubling
 * Used as base case for Newton's method
 * @param a Dividend
 * @param b Divisor
 * @return Quotient of a / b
 */
int2048 force_div(const int2048 &a, const int2048 &b) {
    if (a < b) {
        return 0;
    }
    
    int2048 quotient = 0;
    std::vector<int2048> sums, counts;
    
    sums.reserve(static_cast<int>(std::log2(a.len() - b.len() + 1)) + 2);
    
    // Build powers of b: b, 2b, 4b, 8b, ...
    int2048 power_sum = b;
    int2048 power_count = 1;
    
    while (power_sum <= a) {
        sums.emplace_back(power_sum);
        counts.emplace_back(power_count);
        power_sum += power_sum;
        power_count += power_count;
    }
    
    // Greedily subtract largest powers
    int2048 remainder = a;
    while (!sums.empty()) {
        if (sums.back() <= remainder) {
            remainder -= sums.back();
            quotient += counts.back();
        }
        sums.pop_back();
        counts.pop_back();
    }
    
    return quotient;
}

/**
 * Newton's method to compute reciprocal: 10^{2m} / b
 * Uses Newton iteration: x_{n+1} = 2*x_n - b*x_n^2
 * @param b Divisor
 * @return Approximation of 10^{2m} / b
 */
int2048 newton_inv(const int2048 &b) {
    // Base case: use brute force for small numbers
    if (b.len() <= kLEN * 10) {
        int2048 numerator = 1;
        numerator.left_shift(2 * b.len());
        return force_div(numerator, b);
    }
    
    int m = b.len();
    int k = (m + 1) / 2 + 2;
    
    // Recursively compute inverse of truncated b
    int2048 b_truncated = b;
    b_truncated.right_shift(m - k);
    
    auto inverse_approx = newton_inv(b_truncated);
    
    // Newton iteration step
    auto result = (int2048(2) * inverse_approx).left_shift(m - k) - 
                  (b * inverse_approx * inverse_approx).right_shift(2 * k);
    
    // Correction step
    int2048 numerator = 1;
    numerator.left_shift(2 * m);
    if (numerator <= result * b) {
        result -= 1;
    }
    
    return result;
}

/**
 * Division using Newton's method for reciprocal
 * Computes a / b by first finding 1/b, then multiplying
 * @param a Dividend
 * @param b Divisor
 * @return Quotient of a / b
 */
int2048 div(int2048 a, int2048 b) {
    if (a < b) {
        return 0;
    }
    
    int n = a.len();
    int m = b.len();
    
    // Normalize if a is much larger than b
    if (n > 2 * m) {
        a.left_shift(n - 2 * m);
        b.left_shift(n - 2 * m);
        n = a.len();
        m = b.len();
    }
    
    // Compute reciprocal of b
    int2048 numerator = 1;
    numerator.left_shift(n);
    
    auto b_inverse = newton_inv(b);
    b_inverse.right_shift(2 * m - n);
    
    // Multiply a by reciprocal of b
    auto quotient = a * b_inverse;
    quotient.right_shift(n);
    
    // Correction step
    if (a >= (quotient + int2048(1)) * b) {
        quotient += 1;
    }
    
    return quotient;
}

// ============================================================================
// Division and Modulo Operators
// ============================================================================

int2048 &int2048::operator/=(const int2048 &a) {
    *this = (*this) / a;
    return *this;
}

/**
 * Division operator with sign handling
 * Implements floor division (rounds towards negative infinity)
 */
int2048 operator/(int2048 a, const int2048 &b) {
    if (a == int2048(0)) {
        return 0;
    }
    
    // Same sign: both positive or both negative
    if (a.sign_bit_ == b.sign_bit_) {
        if (a.sign_bit_ == -1) {
            return div(-a, -b);
        } else {
            return div(a, b);
        }
    }
    // Different signs: implement floor division
    else {
        if (a.sign_bit_ == -1) {
            // Negative dividend, positive divisor
            auto quotient = div(-a, b);
            if ((-a) == b * quotient) {
                return -quotient;
            } else {
                return -(quotient + int2048(1));
            }
        } else {
            // Positive dividend, negative divisor
            auto quotient = div(a, -b);
            if (a == (-b) * quotient) {
                return -quotient;
            } else {
                return -(quotient + int2048(1));
            }
        }
    }
}

int2048 &int2048::operator%=(const int2048 &a) {
    *this = (*this) % a;
    return *this;
}

/**
 * Modulo operator
 * Computes remainder: a - (a / b) * b
 */
int2048 operator%(int2048 a, const int2048 &b) {
    return a - (a / b) * b;
}

// ============================================================================
// Utility Functions
// ============================================================================

int int2048::len() const {
    return static_cast<int>(vals_.size());
}

}  // namespace sjtu
