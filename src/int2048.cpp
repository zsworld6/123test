#include "int2048.h"
#include <string>

namespace sjtu {
int2048::int2048() {
    vals_ = std::vector<int>(1, 0);
    sign_bit_ = 1;
}
int2048::int2048(long long x) {
    if (x == 0) {
        vals_ = std::vector<int>(1, 0);
        sign_bit_ = 1;
        return;
    }
    vals_.clear();
    sign_bit_ = 1;
    if (x < 0) {
        x = -x;
        sign_bit_ = -1;
    }
    vals_.reserve(std::log10(x) / kLEN + 1);
    while (x) {
        vals_.emplace_back(x % kBASE);
        x /= kBASE;
    }
}
int2048::int2048(const std::string &s) {
    read(s);
}
int2048::int2048(const int2048 &x) {
    vals_ = x.vals_;
    sign_bit_ = x.sign_bit_;
}
int2048::int2048(const poly &a) {
    *this = to_int2048(a);
}

int2048::~int2048() {
    vals_.clear();
    vals_.shrink_to_fit();
}

int2048::operator double() const {
    double res = 0;
    for (int i = len() - 1; i >= 0; i--) {
        res *= kBASE;
        res += vals_[i];
    }
    res *= sign_bit_;
    return res;
}

int2048::operator std::string() const {
    std::string s = "";
    if ((*this) == int2048(0)) {
        return "0";
    }
    if (sign_bit_ == -1) {
        s = "-";
    }
    s += std::to_string(vals_.back());
    for (int i = len() - 2; i >= 0; i--) {
        std::string t = std::to_string(vals_[i]);
        while (t.size() < kLEN) {
            t = "0" + t;
        }
        s += t;
    }
    return s;
}

void int2048::read(const std::string &s) {
    int lowbit = 0;
    if (s[0] == '-') {
        sign_bit_ = -1;
        lowbit = 1;
    } else {
        sign_bit_ = 1;
    }
    vals_.clear();
    vals_.reserve(s.size() / kLEN + 1);
    for (int i = static_cast<int>(s.size()) - 1; i >= lowbit; i -= kLEN) {
        int res = 0;
        for (int j = std::min(kLEN, i + 1 - lowbit) - 1; j >= 0; j--) {
            res = res * 10 + (s[i - j] - '0');
        }
        vals_.emplace_back(res);
    }
    while (vals_.size() > 1 && vals_.back() == 0) {
        vals_.pop_back();
    }
}
void int2048::print() {
    std::cout << (*this);
}

int2048 add(int2048, const int2048 &);
int2048 minus(int2048, const int2048 &);

int2048 add(int2048 a, const int2048 & b) {
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return -add(-a, -b);
    } else if (b.sign_bit_ == -1) {
        return minus(a, -b);
    } else if (a.sign_bit_ == -1) {
        return minus(b, -a);
    }
    int maxlen = std::max(a.vals_.size(), b.vals_.size());
    a.vals_.resize(maxlen + 1);
    bool carry = 0;
    for (int i = 0; i < static_cast<int>(b.vals_.size()) || carry; i++) {
        if (i < static_cast<int>(b.vals_.size())) {
            a.vals_[i] += b.vals_[i];
        }
        if (a.vals_[i] >= kBASE) {
            a.vals_[i + 1]++;
            a.vals_[i] -= kBASE;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    while (a.vals_.size() > 1 && a.vals_.back() == 0) {
        a.vals_.pop_back();
    }
    return a;
}
int2048 minus(int2048 a, const int2048 &b) {
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return minus(-b, -a);
    } else if (a.sign_bit_ == -1) {
        return -add(-a, b);
    } else if (b.sign_bit_ == -1) {
        return add(a, -b);
    }
    if (a < b) {
        return -minus(b, a);
    }
    a.vals_.resize(a.vals_.size() + 1);
    bool carry = 0;
    for (int i = 0; i < static_cast<int>(b.vals_.size()) || carry; i++) {
        if (i < static_cast<int>(b.vals_.size())) {
            a.vals_[i] -= b.vals_[i];
        }
        if (a.vals_[i] < 0) {
            a.vals_[i] += kBASE;
            a.vals_[i + 1]--;
            carry = 1;
        } else {
            carry = 0;
        }
    }
    while (a.vals_.size() > 1 && a.vals_.back() == 0) {
        a.vals_.pop_back();
    }
    return a;
}

int2048 &int2048::add(const int2048 &b) {
    *this = sjtu::add(*this, b);
    return *this;
}
int2048 &int2048::minus(const int2048 &b) {
    *this = sjtu::minus(*this, b);
    return *this;
}

int2048 int2048::operator+() const {
    return *this;
}
int2048 int2048::operator-() const {
    int2048 ans = *this;
    ans.sign_bit_ = -ans.sign_bit_;
    return ans;
}

int2048 &int2048::operator=(const int2048 &b) {
    (*this).vals_ = b.vals_;
    (*this).sign_bit_ = b.sign_bit_;
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

std::istream &operator>>(std::istream &in, int2048 &a) {
    std::string s;
    in >> s;
    a = int2048(s);
    return in;
}
// @warning: RELY ON kBASE AND kLEN!!!!
std::ostream &operator<<(std::ostream &out, const int2048 &a) {
    if (a.sign_bit_ == -1 && (a.vals_.size() > 1 || a.vals_[0] != 0)) {
        out << '-';
    }
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; i--) {
        if (i == static_cast<int>(a.vals_.size()) - 1) {
            out << a.vals_[i];
            continue;
        }
        if (a.vals_[i] >= 100) {
            out << a.vals_[i];
        } else if (a.vals_[i] >= 10) {
            out << "0" << a.vals_[i];
        } else {
            out << "00" << a.vals_[i];
        }
    }
    return out;
}

bool operator==(const int2048 &a, const int2048 &b) {
    if (a.vals_.size() == 1 && a.vals_.back() == 0 && b.vals_.size() == 1 && b.vals_.back() == 0) {
        return true;
    }
    if (a.sign_bit_ != b.sign_bit_) {
        return false;
    }
    if (a.vals_.size() != b.vals_.size()) {
        return false;
    }
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; i--) {
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
    if (a == int2048(0) && b == int2048(0)) {
        return true;
    }
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return ((-a) >= (-b));
    }
    if (a.sign_bit_ != b.sign_bit_) {
        return a.sign_bit_ < b.sign_bit_;
    }
    if (a.vals_.size() != b.vals_.size()) {
        return a.vals_.size() < b.vals_.size();
    }
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; i--) {
        if (a.vals_[i] > b.vals_[i]) {
            return false;
        } else if (a.vals_[i] < b.vals_[i]) {
            return true;
        }
    }
    return true;
}
bool operator>=(const int2048 &a, const int2048 &b) {
    if (a == int2048(0) && b == int2048(0)) {
        return true;
    }
    if (a.sign_bit_ == -1 && b.sign_bit_ == -1) {
        return ((-a) <= (-b));
    }
    if (a.sign_bit_ != b.sign_bit_) {
        return a.sign_bit_ > b.sign_bit_;
    }
    if (a.vals_.size() != b.vals_.size()) {
        return a.vals_.size() > b.vals_.size();
    }
    for (int i = static_cast<int>(a.vals_.size()) - 1; i >= 0; i--) {
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

constexpr double eps = 1e-7;

poly int2048::to_poly() const {
    poly ans;
    ans.reserve(vals_.size());
    for (int x : vals_) {
        ans.push_back(x);
    }
    while (ans.size() > 1 && std::abs(ans.back().real()) < eps) {
        ans.pop_back();
    }
    return ans;
}
int2048 int2048::to_int2048(const poly &a) const {
    std::vector<long long> res(a.size() + 1);
    for (int i = 0; i < static_cast<int>(a.size()); i++) {
        res[i] += static_cast<long long>(a[i].imag() / 2 + 0.5);
        res[i + 1] += res[i] / kBASE;
        res[i] %= kBASE;
    }
    while (res.size() > 1 && res.back() == 0) {
        res.pop_back();
    }
    int2048 ans;
    ans.vals_.clear();
    ans.vals_.reserve(res.size());
    for (auto x : res) {
        ans.vals_.push_back(x);
    }
    while (ans.vals_.size() > 1 && ans.vals_.back() == 0) {
        ans.vals_.pop_back();
    }
    return ans;
}

namespace polymul {
// high-memory-low-time version
const double kPI = acos(-1);

std::vector<int> rev;
int history_max_len = 1;
std::vector<Complex> pww[35][2];


// butterfly transform
void init(int k) {
    int len = 1 << k;
    if (len > history_max_len) {
        rev.resize(len);
        for (int i = history_max_len; i < len; i <<= 1) {
            int t = std::log2(i);
            Complex wn = exp(Complex(0, kPI / i));
            pww[t][0].resize(i);
            pww[t][0][0] = Complex(1, 0);
            for (int j = 1; j < i; j++)
                pww[t][0][j] = pww[t][0][j - 1] * wn;

            wn = exp(Complex(0, -1 * kPI / i));
            pww[t][1].resize(i);
            pww[t][1][0] = Complex(1, 0);
            for (int j = 1; j < i; j++)
                pww[t][1][j] = pww[t][1][j - 1] * wn;
        }
        history_max_len = len;
    }
    for (int i = 0; i < len; i++) {
        rev[i] = (rev[i >> 1] >> 1) | ((i & 1) << (k - 1));
    }
}

void FFT(poly &a, int n, int inv) {
    for (int i = 0; i < n; i++) {
        if (i < rev[i]) {
            std::swap(a[i], a[rev[i]]);
        }
    }
    for (int mid = 1; mid < n; mid <<= 1) {
        int t = std::log2(mid);
        for (int i = 0; i < n; i += (mid << 1)) {
            for (int j = 0; j < mid; j++) {
                Complex x = a[i + j], y = pww[t][(inv == 1) ? 0 : 1][j] * a[i + j + mid];
                a[i + j] = x + y, a[i + j + mid] = x - y;
            }
        }
    }
    if (inv == -1) {
        for (int i = 0; i < n; i++) {
            a[i] /= n;
        }
    }
}

poly mul(poly a, poly b) {
    int maxlen = std::max(a.size(), b.size());
    int bit = 1, len = 2;
    while ((1 << bit) < 2 * maxlen - 1) {
        bit++;
        len <<= 1;
    }
    init(bit);
    a.resize(len), b.resize(len);
    for (int i = 0; i < len; i++) {
        a[i] = {a[i].real(), b[i].real()};
    }
    FFT(a, len, 1);
    for (int i = 0; i < len; i++) {
        a[i] = a[i] * a[i];
    }
    FFT(a, len, -1);
    return a;
}
}

int2048 operator*(int2048 a, const int2048 &b) {
    auto A = a.to_poly(), B = b.to_poly();
    auto ans = int2048(polymul::mul(A, B));
    ans.sign_bit_ = (a.sign_bit_ == b.sign_bit_ ? 1 : -1);
    return ans;
}
int2048 &int2048::operator*=(const int2048 &b) {
    *this = (*this) * b;
    return *this;
}

int2048 &int2048::left_shift(const int k) {
    vals_.resize(vals_.size() + k);
    for (int i = static_cast<int>(vals_.size()) - 1; i >= k; i--) {
        vals_[i] = vals_[i - k];
    }
    for (int i = 0; i < k; i++) {
        vals_[i] = 0;
    }
    return *this;
}
int2048 &int2048::right_shift(const int k) {
    if (k > static_cast<int>(vals_.size())) {
        return ((*this) = 0);
    }
    for (int i = k; i < static_cast<int>(vals_.size()); i++) {
        vals_[i - k] = vals_[i];
    }
    for (int i = 0; i < k; i++) {
        vals_.pop_back();
    }
    return *this;
}

int2048 force_div(const int2048 &a, const int2048 &b) {
    if (a < b) {
        return 0;
    }
    int2048 ans = 0;
    std::vector<int2048> sums, cnts;
    sums.reserve(std::log2(a.len() - b.len() + 1) + 2);
    int2048 pw_sum = b, pw_cnt = 1;
    while (pw_sum <= a) {
        sums.emplace_back(pw_sum);
        cnts.emplace_back(pw_cnt);
        pw_sum += pw_sum;
        pw_cnt += pw_cnt;
    }
    int2048 tmp = a;
    while (!sums.empty()) {
        if (sums.back() <= tmp) {
            tmp -= sums.back();
            ans += cnts.back();
        }
        sums.pop_back();
        cnts.pop_back();
    }
    return ans;
}
int2048 newton_inv(const int2048 &b) {
    if (b.len() <= kLEN * 10) {
        int2048 t = 1;
        t.left_shift(2 * b.len());
        auto res = force_div(t, b);
        return res;
    }
    int m = b.len();
    int k = (m + 1) / 2 + 2;
    int2048 bb = b;
    bb.right_shift(m - k);
    auto cc = newton_inv(bb);
    auto ans = (int2048(2) * cc).left_shift(m - k) - (b * cc * cc).right_shift(2 * k);
    int2048 t = 1;
    t.left_shift(2 * m);
    if (t <= ans * b) {
        ans -= 1;
    }
    return ans;
}
int2048 div(int2048 a, int2048 b) {
    if (a < b) {
        return 0;
    }
    int n = a.len(), m = b.len();
    if (n > 2 * m) {
        a.left_shift(n - 2 * m);
        b.left_shift(n - 2 * m);
        n = a.len();
        m = b.len();
    }
    int2048 t = 1;
    t.left_shift(n);
    auto binv = newton_inv(b);
    binv.right_shift(2 * m - n);
    auto ans = a * binv;
    ans.right_shift(n);
    if (a >= (ans + int2048(1)) * b) {
        ans += 1;
    }
    return ans;
}

int2048 &int2048::operator/=(const int2048 &a) {
    *this = (*this) / a;
    return *this;
}
int2048 operator/(int2048 a, const int2048 &b) {
    if (a == int2048(0)) {
        return 0;
    }
    if (a.sign_bit_ == b.sign_bit_) {
        if (a.sign_bit_ == -1) {
            return div(-a, -b);
        } else {
            return div(a, b);
        }
    }
    else {
        if (a.sign_bit_ == -1) {
            auto t = div(-a, b);
            if ((-a) == b * t) {
                return -t;
            } else {
                return -(t + int2048(1));
            }
        } else {
            auto t = div(a, -b);
            if (a == (-b) * t) {
                return -t;
            } else {
                return -(t + int2048(1));
            }
        }
    }
}

int2048 &int2048::operator%=(const int2048 &a) {
    *this = (*this) % a;
    return *this;
}
int2048 operator%(int2048 a, const int2048 &b) {
    return a - (a / b) * b;
}

int int2048::len() const {
    return vals_.size();
}

} // namespace sjtu
