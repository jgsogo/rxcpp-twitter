
namespace utils {
    // Taken from https://stackoverflow.com/questions/51695541/stdoptionalvalue-or-lazy-argument-evaluation

    template <typename F>
    struct Lazy
    {
        F f;  

        operator decltype(auto)() const
        {
            return f();
        }
    };

    template <typename F>
    Lazy(F f) -> Lazy<F>;
}

