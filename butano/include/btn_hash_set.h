#ifndef BTN_HASH_SET_H
#define BTN_HASH_SET_H

#include <new>
#include "btn_memory.h"
#include "btn_algorithm.h"
#include "btn_power_of_two.h"
#include "btn_hash_set_fwd.h"

namespace btn
{

template<typename Key, typename KeyHash, typename KeyEqual>
class ihash_set
{

public:
    using key_type = Key;
    using value_type = Key;
    using size_type = size_t;
    using hasher = KeyHash;
    using key_equal = KeyEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    class iterator
    {

    public:
        using key_type = ihash_set::key_type;
        using value_type = ihash_set::value_type;
        using size_type = ihash_set::size_type;
        using hasher = ihash_set::hasher;
        using key_equal = ihash_set::key_equal;
        using reference = ihash_set::reference;
        using const_reference = ihash_set::const_reference;
        using pointer = ihash_set::pointer;
        using const_pointer = ihash_set::const_pointer;

        friend class ihash_set;

        iterator& operator++()
        {
            size_type index = _index;
            size_type last_valid_index = _set->_last_valid_index;
            const bool* allocated = _set->_allocated;
            ++index;

            while(index <= last_valid_index && ! allocated[index])
            {
                ++index;
            }

            if(index > last_valid_index)
            {
                index = _set->max_size();
            }

            _index = index;
            return *this;
        }

        [[nodiscard]] iterator operator++(int)
        {
            iterator result(*this);
            ++result;
            return result;
        }

        iterator& operator--()
        {
            int index = _index;
            int first_valid_index = _set->_first_valid_index;
            const bool* allocated = _set->_allocated;
            --index;

            while(index >= first_valid_index && ! allocated[index])
            {
                --index;
            }

            _index = index;
            return *this;
        }

        [[nodiscard]] iterator operator--(int)
        {
            iterator result(*this);
            --result;
            return result;
        }

        [[nodiscard]] const_reference operator*() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage[_index];
        }

        [[nodiscard]] reference operator*()
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage[_index];
        }

        [[nodiscard]] const_pointer operator&() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        [[nodiscard]] pointer operator&()
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        const_pointer operator->() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        pointer operator->()
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        [[nodiscard]] bool operator==(const iterator& other) const
        {
            return  _index == other._index;
        }

        [[nodiscard]] bool operator!=(const iterator& other) const
        {
            return ! (*this == other);
        }

    private:
        size_type _index;
        ihash_set* _set;

        iterator(size_type index, ihash_set& set) :
            _index(index),
            _set(&set)
        {
        }
    };

    class const_iterator
    {

    public:
        using key_type = ihash_set::key_type;
        using value_type = ihash_set::value_type;
        using size_type = ihash_set::size_type;
        using hasher = ihash_set::hasher;
        using key_equal = ihash_set::key_equal;
        using reference = ihash_set::reference;
        using const_reference = ihash_set::const_reference;
        using pointer = ihash_set::pointer;
        using const_pointer = ihash_set::const_pointer;

        friend class ihash_set;
        friend class iterator;

        const_iterator(const iterator& other):
            _index(other._index),
            _set(other._set)
        {
        }

        const_iterator& operator++()
        {
            size_type index = _index;
            size_type last_valid_index = _set->_last_valid_index;
            const bool* allocated = _set->_allocated;
            ++index;

            while(index <= last_valid_index && ! allocated[index])
            {
                ++index;
            }

            if(index > last_valid_index)
            {
                index = _set->max_size();
            }

            _index = index;
            return *this;
        }

        [[nodiscard]] const_iterator operator++(int)
        {
            const_iterator result(*this);
            ++result;
            return result;
        }

        const_iterator& operator--()
        {
            int index = _index;
            int first_valid_index = _set->_first_valid_index;
            const bool* allocated = _set->_allocated;
            --index;

            while(index >= first_valid_index && ! allocated[index])
            {
                --index;
            }

            _index = index;
            return *this;
        }

        [[nodiscard]] const_iterator operator--(int)
        {
            const_iterator result(*this);
            --result;
            return result;
        }

        [[nodiscard]] const_reference operator*() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage[_index];
        }

        [[nodiscard]] const_pointer operator&() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        const_pointer operator->() const
        {
            BTN_ASSERT(_set->_allocated[_index], "Index is not allocated: ", _index);

            return _set->_storage + _index;
        }

        [[nodiscard]] bool operator==(const const_iterator& other) const
        {
            return  _index == other._index;
        }

        [[nodiscard]] bool operator!=(const const_iterator& other) const
        {
            return ! (*this == other);
        }

    private:
        size_type _index;
        const ihash_set* _set;

        const_iterator(size_type index, const ihash_set& set) :
            _index(index),
            _set(&set)
        {
        }
    };

    friend class iterator;
    friend class const_iterator;

    [[nodiscard]] iterator begin()
    {
        return iterator(_first_valid_index, *this);
    }

    [[nodiscard]] const_iterator begin() const
    {
        return const_iterator(_first_valid_index, *this);
    }

    [[nodiscard]] const_iterator cbegin() const
    {
        return const_iterator(_first_valid_index, *this);
    }

    [[nodiscard]] iterator end()
    {
        return iterator(_max_size, *this);
    }

    [[nodiscard]] const_iterator end() const
    {
        return const_iterator(_max_size, *this);
    }

    [[nodiscard]] const_iterator cend() const
    {
        return const_iterator(_max_size, *this);
    }

    [[nodiscard]] size_type size() const
    {
        return _size;
    }

    [[nodiscard]] size_type max_size() const
    {
        return _max_size;
    }

    [[nodiscard]] size_type available() const
    {
        return _max_size - _size;
    }

    [[nodiscard]] bool empty() const
    {
        return _size == 0;
    }

    [[nodiscard]] bool full() const
    {
        return _size == _max_size;
    }

    [[nodiscard]] bool contains(const key_type& key) const
    {
        if(empty())
        {
            return false;
        }

        return contains_hash(hasher()(key), key);
    }

    [[nodiscard]] bool contains_hash(size_type key_hash, const key_type& key) const
    {
        return find_hash(key_hash, key) != end();
    }

    [[nodiscard]] size_type count(const key_type& key) const
    {
        return count_hash(hasher()(key), key);
    }

    [[nodiscard]] size_type count_hash(size_type key_hash, const key_type& key) const
    {
        return contains_hash(key_hash, key) ? 1 : 0;
    }

    [[nodiscard]] const_iterator find(const key_type& key) const
    {
        return const_cast<ihash_set&>(*this).find(key);
    }

    [[nodiscard]] iterator find(const key_type& key)
    {
        if(empty())
        {
            return end();
        }

        return find_hash(hasher()(key), key);
    }

    [[nodiscard]] const_iterator find_hash(size_type key_hash, const key_type& key) const
    {
        return const_cast<ihash_set&>(*this).find_hash(key_hash, key);
    }

    [[nodiscard]] iterator find_hash(size_type key_hash, const key_type& key)
    {
        if(empty())
        {
            return end();
        }

        const_pointer storage = _storage;
        const bool* allocated = _allocated;
        key_equal key_equal_functor;
        size_type index = _index(key_hash);
        size_type max_size = _max_size;
        size_type its = 0;

        while(its < max_size && allocated[index])
        {
            if(key_equal_functor(key, storage[index]))
            {
                return iterator(index, *this);
            }

            index = _index(index + 1);
            ++its;
        }

        return end();
    }

    iterator insert(const value_type& value)
    {
        return insert_hash(hasher()(value), value);
    }

    iterator insert(value_type&& value)
    {
        return insert_hash(hasher()(value), move(value));
    }

    template<typename KeyType, typename ValueType>
    iterator insert(KeyType&& key, ValueType&& value)
    {
        return insert_hash(hasher()(key), value_type(forward<KeyType>(key), forward<ValueType>(value)));
    }

    iterator insert_hash(size_type key_hash, const value_type& value)
    {
        return insert_hash(key_hash, value_type(value));
    }

    iterator insert_hash(size_type key_hash, value_type&& value)
    {
        BTN_ASSERT(! full(), "Hash set is full");

        size_type index = _index(key_hash);
        pointer storage = _storage;
        bool* allocated = _allocated;
        key_equal key_equal_functor;
        size_type current_index = index;

        while(allocated[current_index])
        {
            if(key_equal_functor(value, storage[current_index]))
            {
                return end();
            }

            current_index = _index(current_index + 1);
            BTN_ASSERT(current_index != index, "All indices are allocated");
        }

        ::new(storage + current_index) value_type(move(value));
        allocated[current_index] = true;
        _first_valid_index = min(_first_valid_index, current_index);
        _last_valid_index = max(_last_valid_index, current_index);
        ++_size;
        return iterator(current_index, *this);
    }

    template<typename KeyType, typename ValueType>
    iterator insert_hash(size_type key_hash, KeyType&& key, ValueType&& value)
    {
        return insert_hash(key_hash, value_type(forward<KeyType>(key), forward<ValueType>(value)));
    }

    iterator insert_or_assign(const value_type& value)
    {
        return insert_or_assign_hash(hasher()(value), value);
    }

    iterator insert_or_assign(value_type&& value)
    {
        return insert_or_assign_hash(hasher()(value), move(value));
    }

    template<typename KeyType, typename ValueType>
    iterator insert_or_assign(KeyType&& key, ValueType&& value)
    {
        return insert_or_assign_hash(hasher()(key), value_type(forward<KeyType>(key), forward<ValueType>(value)));
    }

    iterator insert_or_assign_hash(size_type key_hash, const value_type& value)
    {
        return insert_or_assign_hash(key_hash, value_type(value));
    }

    iterator insert_or_assign_hash(size_type key_hash, value_type&& value)
    {
        iterator it = find_hash(key_hash, value);

        if(it == end())
        {
            it = insert_hash(key_hash, move(value));
            BTN_ASSERT(it != end(), "Insertion failed");
        }
        else
        {
            pointer storage = _storage;
            size_type index = it._index;
            storage[index].~value_type();
            ::new(storage + index) value_type(move(value));
        }

        return it;
    }

    template<typename KeyType, typename ValueType>
    iterator insert_or_assign_hash(size_type key_hash, KeyType&& key, ValueType&& value)
    {
        return insert_or_assign_hash(key_hash, value_type(forward<KeyType>(key), forward<ValueType>(value)));
    }

    iterator erase(const_iterator pos)
    {
        BTN_ASSERT(! empty(), "Hash set is empty");

        bool* allocated = _allocated;
        size_type index = pos._index;
        BTN_ASSERT(allocated[index], "Index is not allocated: ", index);

        pointer storage = _storage;
        storage[index].~value_type();
        allocated[index] = false;
        --_size;

        if(_size == 0)
        {
            _first_valid_index = _max_size;
            _last_valid_index = 0;
            return end();
        }

        if(_size == 1)
        {
            if(index == _first_valid_index)
            {
                index = _last_valid_index;
                _first_valid_index = _last_valid_index;
            }
            else
            {
                index = _first_valid_index;
                _last_valid_index = _first_valid_index;
            }

            return iterator(index, *this);
        }

        hasher hasher_functor;
        size_type current_index = index;
        size_type next_index = _index(index + 1);

        while(allocated[next_index] && _index(hasher_functor(storage[next_index])) != next_index)
        {
            ::new(storage + current_index) value_type(move(storage[next_index]));
            storage[next_index].~value_type();
            allocated[current_index] = true;
            allocated[next_index] = false;
            current_index = next_index;
            next_index = _index(next_index + 1);
        }

        size_type first_valid_index = _first_valid_index;

        while(! allocated[first_valid_index])
        {
            ++first_valid_index;
        }

        _first_valid_index = first_valid_index;

        size_type last_valid_index = _last_valid_index;

        while(! allocated[last_valid_index])
        {
            --last_valid_index;
        }

        _last_valid_index = last_valid_index;

        while(index <= last_valid_index && ! allocated[index])
        {
            ++index;
        }

        return iterator(index, *this);
    }

    bool erase(const key_type& key)
    {
        return erase_hash(hasher()(key), key);
    }

    bool erase_hash(size_type key_hash, const key_type& key)
    {
        iterator it = find_hash(key_hash, key);

        if(it != end())
        {
            erase(it);
            return true;
        }

        return false;
    }

    void clear()
    {
        if(_size)
        {
            pointer storage = _storage;
            bool* allocated = _allocated;
            size_type first_valid_index = _first_valid_index;
            size_type last_valid_index = _last_valid_index;

            for(size_type index = first_valid_index; index <= last_valid_index; ++index)
            {
                if(allocated[index])
                {
                    storage[index].~value_type();
                }
            }

            size_type max_size = _max_size;
            memory::clear(int(max_size), *allocated);
            _first_valid_index = max_size;
            _last_valid_index = 0;
            _size = 0;
        }
    }

    bool operator==(const ihash_set& other) const
    {
        size_type first_valid_index = _first_valid_index;
        size_type last_valid_index = _last_valid_index;

        if(_size != other._size || first_valid_index != other._first_valid_index ||
                last_valid_index != other._last_valid_index)
        {
            return false;
        }

        const_pointer storage = _storage;
        const_pointer other_storage = other._storage;
        const bool* allocated = _allocated;
        const bool* other_allocated = other._allocated;

        for(size_type index = first_valid_index; index <= last_valid_index; ++index)
        {
            if(allocated[index] != other_allocated[index])
            {
                return false;
            }

            if(allocated[index] && storage[index] != other_storage[index])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const ihash_set& other) const
    {
        return ! (operator==(other));
    }

protected:
    ihash_set(reference storage, bool& allocated, size_type max_size) :
        _storage(&storage),
        _allocated(&allocated),
        _max_size(max_size),
        _first_valid_index(max_size)
    {
        BTN_ASSERT(power_of_two(max_size), "Max size is not power of two: ", max_size);
    }

private:
    pointer _storage;
    bool* _allocated;
    size_type _max_size;
    size_type _first_valid_index;
    size_type _last_valid_index = 0;
    size_type _size = 0;

    void _assign(const ihash_set& other)
    {
        const_pointer other_storage = other._typed_storage();
        pointer storage = _storage;
        bool* allocated = _allocated;
        size_type first_valid_index = other._first_valid_index;
        size_type last_valid_index = other._last_valid_index;
        memory::copy(*other._allocated, _max_size, *allocated);

        for(size_type index = first_valid_index; index <= last_valid_index; ++index)
        {
            if(allocated[index])
            {
                ::new(storage + index) value_type(other_storage[index]);
            }
        }

        _first_valid_index = other._first_valid_index;
        _last_valid_index = other._last_valid_index;
        _size = other._size;
    }

    void _assign(ihash_set&& other)
    {
        pointer other_storage = other._typed_storage();
        pointer storage = _storage;
        bool* allocated = _allocated;
        size_type first_valid_index = other._first_valid_index;
        size_type last_valid_index = other._last_valid_index;
        memory::copy(*other._allocated, _max_size, *allocated);

        for(size_type index = first_valid_index; index <= last_valid_index; ++index)
        {
            if(allocated[index])
            {
                ::new(storage + index) value_type(move(other_storage[index]));
            }
        }

        _first_valid_index = other._first_valid_index;
        _last_valid_index = other._last_valid_index;
        _size = other._size;

        other.clear();
    }

    size_type _index(size_type key_hash) const
    {
        return key_hash & (_max_size - 1);
    }
};


template<typename Key, size_t MaxSize, typename KeyHash, typename KeyEqual>
class hash_set : public ihash_set<Key, KeyHash, KeyEqual>
{
    static_assert(power_of_two(MaxSize));

public:
    using key_type = Key;
    using value_type = Key;
    using size_type = size_t;
    using hasher = KeyHash;
    using key_equal = KeyEqual;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    hash_set() :
        ihash_set<Key, KeyHash, KeyEqual>(*reinterpret_cast<pointer>(_storage_buffer), *_allocated_buffer, MaxSize)
    {
    }

    hash_set(const hash_set& other)
    {
        _assign(other);
    }

    hash_set& operator=(const hash_set& other)
    {
        if(this != &other)
        {
            this->clear();
            _assign(other);
        }

        return *this;
    }

    hash_set(hash_set&& other)
    {
        _assign(move(other));
    }

    hash_set& operator=(hash_set&& other)
    {
        if(this != &other)
        {
            this->clear();
            _assign(move(other));
        }

        return *this;
    }

    ~hash_set()
    {
        this->clear();
    }

private:
    alignas(alignof(value_type)) char _storage_buffer[sizeof(value_type) * MaxSize];
    bool _allocated_buffer[MaxSize] = { false };
};

}

#endif
