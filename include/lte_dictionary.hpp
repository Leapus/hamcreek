#pragma once

/*
A sorted lookup table that returns the closest element which is less than or equal to the sought key.
Added this so that a numeric power level can be slotted to the closest option available for a device which provides
only a set of specific options, like: Low, Medium, and comically enough, "Small".
*/

#include <initializer_list>
#include <exception>
#include <map>

namespace leapus{

template<typename K, typename T>
class lte_dictionary{
public:
    using key_type=K;
    using value_type=T;

private:
    using container_type = std::map<K,T>;
    container_type m_container;

public:
    lte_dictionary(std::initializer_list<typename container_type::value_type> elements):
        m_container(elements){
    }

    const value_type &get(const K &k) const{
        auto it=m_container.lower_bound(k);

        //If no elements are equal or greater, then return the last/largest one
        //Or throw if the container is empty
        if(it==m_container.end()){
            if(m_container.size() == 0)
                throw std::range_error("Not found");
            else
                return (--m_container.end())->second;
        }

        //It's the first element not-less, so it could be equal or greater
        if(it->first > k){
            //If it's greater, then grab the previous element, unless there is none, then that's an error
            if(it==m_container.begin())
                throw std::range_error("Not found");
            else
                --it;
        }
        //Else it's equal and that's an exact match
        return it->second;
    }

    const value_type &operator[](const K &k) const{
        return get(k);
    }

};

}