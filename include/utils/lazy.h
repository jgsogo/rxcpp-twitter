
namespace utils {
    
    // Taken from https://stackoverflow.com/questions/51695541/stdoptionalvalue-or-lazy-argument-evaluation
    //  previous attemp: https://foonathan.net/blog/2017/06/27/lazy-evaluation.html
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

