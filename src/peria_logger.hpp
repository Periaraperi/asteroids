#pragma once

#ifdef PERIA_DEBUG
    #include <iostream>
    
    inline
    void peria_log()
    { std::cerr << '\n'; }

    template <typename Head, typename... Tail>
    void peria_log(Head&& h, Tail&&... t)
    {
        std::cerr << std::forward<Head>(h);
        peria_log(std::forward<Tail>(t)...);
    }
    
    #define PERIA_LOG(...) peria_log(__VA_ARGS__)
#else
    #define PERIA_LOG(...)
#endif


