/*

CPS confuiguration is divided into various files, so here we divide configuration
into corresponding sections.

*/

#pragma once
#include <tuple>

#include "algo.hpp"

namespace leapus::hamconf{


//Takes a tuple of member pointers enumerating the fields in a configuration object,
//providing a visitor interface to visit the configuration fields.
template<typename Tuple>
class member_enumerator;

//Takes a visitor via visit() and calls it with each member pointer.
//The visitor will need to hold a reference or pointer to the containing object.
template<class C, typename... T>
class member_enumerator<std::tuple< T C::*...>>{
    using tuple_type = std::tuple< T C::*...>;
    const tuple_type &m_tuple;

    template<typename V>
    struct each_functor{
        V &visitor;
        each_functor(V &v):visitor(v){}

        template<typename T2>
        T2 operator()(T2 C::*p){
            visitor(p->*p);
        }
    };

public:
    static constexpr unsigned int member_count = std::tuple_size<tuple_type>::value;

    member_enumerator(const tuple_type &tuple):m_tuple(tuple){}

    template<typename V>
    void visit(V &visitor){
        algo::for_each(m_tuple, each_functor<V>(visitor));
    }
};

template<class C, typename... T>
auto make_member_enumerator( const std::tuple<T C::*...> &members){
    return member_enumerator<std::tuple<T C::*...>>(members);
}

}
