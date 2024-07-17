template<typename T>
struct Equals {
    T value;
    constexpr bool operator()(T x) const { return x == value; }
};

template<typename T>
struct NotEquals {
    T value;
    constexpr bool operator()(T x) const { return x != value; }
};

template<typename T>
struct GreaterThan {
    T value;
    constexpr bool operator()(T x) const { return x > value; }
};

template<typename T>
struct LessThan {
    T value;
    constexpr bool operator()(T x) const { return x < value; }
};

template<typename LHS, typename RHS>
struct And {
    LHS lhs;
    RHS rhs;
    constexpr bool operator()(auto x) const { return lhs(x) && rhs(x); }
};

template<typename LHS, typename RHS>
struct Or {
    LHS lhs;
    RHS rhs;
    constexpr bool operator()(auto x) const { return lhs(x) || rhs(x); }
};

template<typename Expr>
struct Not {
    Expr expr;
    constexpr bool operator()(auto x) const { return !expr(x); }
};

// ======================
// Helper Functions
// ======================

template<typename T>
constexpr auto eq(T value) {
    return Equals<T>{value};
}

template<typename T>
constexpr auto neq(T value) {
    return NotEquals<T>{value};
}

template<typename T>
constexpr auto gt(T value) {
    return GreaterThan<T>{value};
}

template<typename T>
constexpr auto lt(T value) {
    return LessThan<T>{value};
}

template<typename LHS, typename RHS>
constexpr auto and_(LHS lhs, RHS rhs) {
    return And<LHS, RHS>{lhs, rhs};
}

template<typename LHS, typename RHS>
constexpr auto or_(LHS lhs, RHS rhs) {
    return Or<LHS, RHS>{lhs, rhs};
}

template<typename Expr>
constexpr auto not_(Expr expr) {
    return Not<Expr>{expr};
}
