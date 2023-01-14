/* Hash Table header.
* WangXiaochun
* zjjhwxc(at)gmail.com
* April 4, 2013
*/

#ifndef TABLE_H
#define TABLE_H

#include "exception.h"
#include "lex.h"
#include "tools.h"

using std::pair;

namespace NS_TABLE
{
    const size_t BASE_SIZE = 29;
    const size_t INCR_SIZE = 10;

    template <class T>
    class HASH
    {
    public:
        static unsigned long hash(T& t)
        {
            return t->getHash();
        }
    };


    template <>
    class HASH<const char*>
    {
    public:
        static unsigned long hash(const char* t)
        {
            return utils::strHash(t);
        }
    };

    template <class K>
    class Table_iter : public iterator<std::random_access_iterator_tag, K, unsigned long, K*, K>
    {
        const K* kPtr;

    public:
        typedef iterator<std::random_access_iterator_tag, K> iter;

        Table_iter()
        {
            kPtr = nullptr;
        }

        Table_iter(const K* k) : kPtr(k) {}

        operator bool() const
        {
            return kPtr == nullptr;
        }

        const K& operator*(void) const
        {
            return *kPtr;
        }
    };
    template <class K, class T>
    class CMP
    {
    public:
        static bool cmp(const K&, const T&)
        {
            return false;
        }
    };


    template <class K, class T, class KHASH = HASH<K>, class THASH = HASH<T>, class C = CMP<K, T> >
    class Table : public Table_iter<K>
    {
        typedef Table<K, T, KHASH, THASH> base;
        typedef unsigned long ul;
        typedef K item;
        //typedef pair<TableException, const KHASH&> retType;

        //static TableException noExc;

        item* tbl;
        base* up;
        base* down;
        size_t tableSize;
        size_t stored;
        size_t threshold;

        void setZero(size_t elementsNo)
        {
            size_t i = 0;
            for (; i < elementsNo; i++)
                *(tbl + i) = 0;
        }

        void rePut()
        {
            item* old = tbl;
            tbl = new item[tableSize + INCR_SIZE];
            setZero(tableSize + INCR_SIZE);
            size_t i = 0;
            for (; i < tableSize; i++) {
                if ((old + i) == nullptr)
                    continue;
                else {
                    PutIntoTable(*(tbl + i));
                }
            }
            tableSize += INCR_SIZE;

            threshold = (tableSize >> 2) + (tableSize >> 1);  // 3/4
        }

        void inline checkSize(void)
        {
            if (stored >= threshold)
                rePut();
        }

        void PutIntoTable(item& it)
        {
            ul index = KHASH::hash(it) % tableSize;
        put:
            if (tbl + index == nullptr) {
                *(tbl + index) = it;
                stored++;
            } else {
                index++;
                goto put;
            }
            checkSize();
        }

        item find(const T& t)
        {
            if (this->stored == 0)
                return nullptr;
            ul hash = THASH::hash(t);
            ul index = hash % tableSize;
            ul i = index;
        fi:
            if (*(tbl + index) != nullptr) {
                if (KHASH::hash(*(tbl + index)) == hash && C::cmp(*(tbl + index), t))
                    return *(tbl + index);
            } else {
                index++;
                if (*(tbl + index) == nullptr)
                    return nullptr;
                if (index == tableSize) {
                    index = 0;
                }
                if (index == i) {
                    return nullptr;
                }
                goto fi;
            }
            return nullptr;
        }

    public:
        explicit Table(size_t _base_size = BASE_SIZE)
        {
            stored = 0;
            tableSize = _base_size;
            tbl = new item[_base_size];
            setZero(_base_size);
            threshold = (tableSize >> 2) + (tableSize >> 1);  // 3/4
            up = down = nullptr;
        }

        void PutIntoTable(K& k, unsigned long hashCode)
        {
            ul index = hashCode % tableSize;
        put:
            if (tbl + index == nullptr)
                *(tbl + index) = k;
            else {
                index++;
                goto put;
            }
            checkSize();
            stored++;
        }

        void PutIntoTable(K k, T t)
        {
            ul hashCode = THASH::hash(t);
            ul index = hashCode % tableSize;
        put:
            if (*(tbl + index) == nullptr)
                *(tbl + index) = k;
            else {
                index++;
                goto put;
            }
            checkSize();
            stored++;
        }

        const item existAllScope(const T& t)
        {
            const item it = existCurrentScope(t);
            if (it != nullptr)
                return it;
            else if (up != nullptr) {
                return up->existAllScope(t);
            } else
                return nullptr;
        }

        const item existCurrentScope(const T& t)
        {
            return find(t);
        }

        auto begin(void)
        {
            item* firstItem = tbl;
            for (int i = 0; i < tableSize; i++)
                if (firstItem != nullptr)
                    break;
            return Table_iter(&(firstItem->first.first));
        }

        size_t size(void) const
        {
            return this->stored;
        }

        void clear(void)
        {
            for (int i = 0; i < tableSize; i++)
                *(tbl + i) = nullptr;
        }

        void enterBlock(base* nextTable)
        {
            this->down = nextTable;
            nextTable->up = this;
        }

        void exitBlock(void)
        {
            if (this->up != nullptr) {
                this->up->down = nullptr;
                this->up = nullptr;
            }
        }
    };

};  // namespace NS_TABLE
#endif