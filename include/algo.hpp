/* Algorithmic stuff */

#pragma once
#include <type_traits>
#include <tuple>

namespace leapus::algo{

template<typename C>
void for_each(const C &col);

//Like void_t, but maps to an empty type, not void
template<typename... T>
struct empty{};

namespace impl{

    template<typename T, typename F, unsigned int I>
    void tuple_for_each( const T &tup, F &func, std::enable_if<I < std::tuple_size<T>::value, int> last=0){
        func( std::get<I>() );
    }

    template<typename T, typename F, unsigned int I>
    void tuple_for_each( const T &tup, F &func, std::enable_if<I >= std::tuple_size<T>::value, bool> last=true){}

    /*
    template<typename T, typename F, unsigned int I>
    struct for_each_impl{
        void do(const T &tup, F &func){
            func(std::get<I>(tup));
        }
    };

    template<typename ... T, typename F>
    struct for_each_impl<std::tuple<T...>,F, std::tuple_size< std::tuple<T...> >::value >{
        void do(const std::tuple<T...> &tup, F &func){}
    };
    */
}

template<typename F, typename ...T>
void for_each(const std::tuple<T...> &tup, F &func){
    impl::tuple_for_each(tup, func);
} 

}