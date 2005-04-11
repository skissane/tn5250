#pragma once

// Abnormal map that works in my case.
template<typename TKey, typename TValue, typename less = std::less<TKey>, typename equal = std::equal_to<TKey> >
class FunkyMap : protected std::vector< std::pair<TKey, TValue> >
{
public:
    typedef TKey                    Key;
    typedef TValue                  Value;

    less  m_less;
    equal m_equal;

    typedef std::vector< std::pair<TKey, TValue> > Parent;
    typedef Parent::iterator        iterator;
    typedef Parent::const_iterator  const_iterator;

    // Insert ordered
    void insert(const Key & k, const Value & v) {
        super::insert(less(k), std::pair<Key, Value>(k, v));
    }

    template<class Comp>
    const_iterator find(const Key & k) const {
        Comp c;
        Comp::result_type r = Comp::max;
        const_iterator iend = end();
        const_iterator best_match = end();
        for (const_iterator itr = begin(); itr != iend; ++itr)
        {
            // Faut trouver le + proche... damn.
            Comp::result_type i = c(itr->first, k);
            
            // Exact match
            if (!i)
                return itr;
            
            // Better match than last found
            if (i < r)
            {
                r = i;
                best_match = itr;
            }
        }
        return best_match;
    }
    iterator less(const Key & k) {
        iterator itr = begin();
        const_iterator iend = end();
        for (; itr != iend && m_less(itr->first, k); ++itr)
            ;
        return itr;
    }

    using Parent::begin;
    using Parent::end;

    void set(const Key & k, const Value & v) {
        iterator itr = find(k);
        if (itr != end())
            itr->second = v;
        else
            insert(k, v);
    }
    void erase(const Key & k)
    {
        iterator itr = find(k);
        if (itr != end())
            super::erase(itr);
    }
protected:
    // Strict find
    iterator find(const Key & k) {
        const_iterator iend = end();
        iterator itr = begin();
        for (; itr != iend; ++itr)
        {
            if (m_equal(itr->first, k))
                break;
        }
        return itr;
    }

    const_iterator find(const Key & k) const {
        const_iterator iend = end();
        const_iterator itr = begin()
        for (; itr != iend; ++itr)
        {
            if (m_equal(itr->first, k))
                break;
        }
        return itr;
    }
};
