
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// declaration ADS_SET
template <typename Key, size_t N = 3>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = key_type & ;
    using const_reference = const key_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_equal = std::equal_to<key_type>;
    using hasher = std::hash<key_type>;
    using key_compare = std::less<key_type>;


private:
    struct Box {
        Box() {
            this->hasData = false;
            this->nextBox = nullptr;
            this -> head=nullptr;
        }
        ~Box() {
                delete nextBox;
            delete head;

        }

        size_t hasData;
        key_type data;
        Box* nextBox;
        Box* head;


    };

    Box* boxes = nullptr;
    size_type tableSize{N};
    size_type maxSize;



/*--------------------------
 ADS_SET - CONSTRUCTORS
 *---------------------------*/
public:
ADS_set() {
    initializeBoxes();
}

ADS_set(std::initializer_list<key_type> ilist) : ADS_set() {
    insert(ilist);
}

template<typename InputIt> ADS_set(InputIt first, InputIt last) : ADS_set() {
    insert(first, last);
}

~ADS_set() {
    freeMemory();
}

    friend bool operator==(const ADS_set& lhs, const ADS_set& rhs) {
        if (lhs.size() != rhs.size()) return false;
        for (const auto& k : lhs)
            if (!rhs.count(k)) return false;
        return true;
    }

    friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs) {
        return !(lhs == rhs);
    }

    ADS_set(const ADS_set& other) : ADS_set(){
        for(auto& i: other)
            insert_unchecked(i);
    }

        void rehash() {
            size_t oldsz = tableSize;
            Box *old_tbl = boxes;
            tableSize*=2;
            boxes = new Box[tableSize];
            maxSize = 0;
            Box * temp = nullptr ;
            for (size_t i = 0; i < oldsz; i++) {
                temp = old_tbl[i].head;
                if (temp != nullptr) {
                    while (temp != nullptr) {
                        insert_unchecked(temp->data);
                        temp = temp->nextBox;
                    }
                }
            }
            delete[] old_tbl;

    }

    iterator find(const key_type& key) const{
        size_t idx = hash_index(key);
        for(auto it = boxes[idx].head; it != nullptr; it = it->nextBox){
            if(key_equal{}(it->data, key)){
                return iterator(it, tableSize, idx, boxes);}
        }
        return end();
    }

    std::pair<iterator, bool> insert(const key_type& key){
        if(contains_private(key)) return make_pair(find(key), false);
        insert_unchecked(key);
        return make_pair(find(key), true);
    }


    ADS_set& operator=(const ADS_set& other){
        initializeBoxes();
        for(auto& i: other)
            insert_unchecked(i);
        return *this;
    }
    ADS_set& operator=(std::initializer_list<key_type> ilist){
        initializeBoxes();
        insert(ilist);
        return *this;
    }

    void swap(ADS_set& other){
        std::swap(tableSize, other.tableSize);
        std::swap(maxSize, other.maxSize);
        std::swap(boxes, other.boxes);
    }

    size_type erase(const key_type& key){
        size_type idx = hash_index(key);
        Box* cur = boxes[idx].head;
        Box* cur2 = nullptr;

        for(; cur!=nullptr; cur= cur->nextBox){
            if(key_equal{}(cur->data,key)){
                if(cur == boxes[idx].head && cur -> nextBox == nullptr){
                    delete boxes[idx].head;
                    boxes[idx].head = nullptr;
                    boxes[idx].hasData--;


                    maxSize = maxSize-1;

                    return 1;
                }
                else if(cur == boxes[idx].head){
                    boxes[idx].head = cur -> nextBox;
                    boxes[idx].hasData--;

                    cur -> nextBox = nullptr;


                    delete cur;
                    --maxSize;
                    return 1;
                }
                else{
                cur2 -> nextBox = cur->nextBox;
                cur -> nextBox = nullptr;
                    boxes[idx].hasData--;

                delete cur;
                cur2 = nullptr;
                delete cur2;
                    maxSize--;
                    return 1;
                }
                }
            if(cur2 != cur){
                cur2 = cur;
            }
        }


        return 0;
    }

    const_iterator begin() const{
        for(size_type i = 0; i < tableSize; i++){
            if(boxes[i].hasData)
                return const_iterator(boxes[i].head, tableSize, i, boxes);
        }
        return end();
    }

    const_iterator end() const { return const_iterator(nullptr); }

/*--------------------------
 ADS_SET - INSERTION & DELETION
 *---------------------------*/

void clear()
{
    initializeBoxes();
}

void insert(std::initializer_list<key_type> ilist) {
    for (const auto& key : ilist) {
        if (contains_private(key))
            continue;
        insert_unchecked(key);
    }
}

template <typename InputIt> void insert(InputIt first, InputIt last) {
    auto it = first;
    while (it != last) {
        if (!contains_private(*it))
            insert_unchecked(*it);
        it++;
    }
}

size_type size() const
{
    return maxSize;
}

bool empty() const
{
    return maxSize == 0;
}

size_type count(const key_type& key) const
{
    return contains_private(key) ? 1 : 0;
}

/*--------------------------
 ADS_SET - OTHER STUFF
 *---------------------------*/

void dump(std::ostream& o = std::cerr) const
{
    for (size_type i = 0; i < tableSize; i++) {
        bool hasOutput = false;
        o << "[" << i << "] ";

        for(auto* box = boxes[i].head; box!=nullptr; box = box->nextBox){
            if (hasOutput)
                o << " -> ";

            o << box->data;
            hasOutput = true;

        }
        o << std::endl;
    }
}

void freeMemory()
{
    maxSize = 0;
    tableSize = 0;

    if (boxes == nullptr)
        return;

    delete[] boxes;
    boxes = nullptr;
}

void initializeBoxes()
{
    freeMemory();
    boxes = new Box[N];
    tableSize = N;
}

size_type hash_index(const key_type& k) const {
    return hasher{}(k) % tableSize;
}

bool contains_private(const key_type& key) const
{
    size_type i = hash_index(key);
        auto* box = boxes[i].head;
        while (box != nullptr) {
            if (key_equal{}(key, box->data))
                return true;
            box = box->nextBox;
        }


    return false;
}

/*
bool y(const key_type& a, const key_type& b){
    if(key_equal{}(a,b)) return false;
        for(size_t i = 0; i < tableSize; i++){
            for(auto j = boxes[i].head; j; j = j->nextBox){
            if(key_equal{}(a,j->data) || key_equal{}(b,j->data)){
                if(j->nextBox){
                    if(key_equal{}(j->nextBox->data, a) || key_equal{}(j->nextBox->data, b)){
                    return true;
                    }
                }else if(i + 1 < tableSize){
                    while(boxes[++i].head == nullptr && i < tableSize);
                    if(i == tableSize) return false;
                    if(key_equal{}(boxes[i].head->data, a) || key_equal{}(boxes[i].head->data, b))
                    return true;
                } else return false;
                }
            }
        }
        return false;
    }*/
/////////
bool y(const key_type& a, const key_type& b) const{
    if(key_equal{}(a,b)) return false;
for(size_t i = 0; i < tableSize; i++){
for(auto j = boxes[i].head; j; j = j->nextBox){
    if(key_equal{}(a,j->data) || key_equal{}(b,j->data)){
        if(j->nextBox){
            if(key_equal{}(j->next->data, a) || key_equal{}(j->next->data, b) ){
                    return true;
                    }
                }else if(i + 1 < tableSize){
                    while(table[++i].head == nullptr && i < max_sz);
                    if(i == max_sz) return false;
                    if(key_equal{}(boxes[i].head->data, a) || key_equal{}(boxes[i].head->data, b))
                    return true;
                } else return false;
            }
        }
}
return false;
}


void insert_unchecked(const key_type& key) {
    auto index = hash_index(key);
    Box* neu = new Box;
    neu -> nextBox = boxes[index].head;
    neu -> data =key;
    boxes[index].head = neu;
    boxes[index].hasData++;
    neu = nullptr;
    maxSize++;
    if(tableSize*5 < maxSize) rehash();
}
};

template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
private:
    Box* box_pointer;
    size_t table_sz;
    size_t position;
    Box* table;


public:
    explicit Iterator(Box* box_pointer = nullptr,
                      size_t table_sz = 0,
                      size_t position = 0,
                      Box* table = nullptr): box_pointer(box_pointer), table_sz(table_sz), position(position), table(table) {}

    reference operator*() const {
        return box_pointer->data;
    }
    pointer operator->() const {
        return &box_pointer -> data;
    }

    Iterator& operator++() {
        if(box_pointer -> nextBox != nullptr){
            box_pointer = box_pointer->nextBox;
            return *this;
        }
        position++;
        while(position < table_sz && table[position].hasData == 0){position++;}
        if(position == table_sz){
            box_pointer = nullptr;
            return *this;
        }

        box_pointer = table[position].head;
        return *this;
    }


    Iterator operator++(int) {
        auto it = *this;
        ++*this;
        return it;
    }
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
        return lhs.box_pointer == rhs.box_pointer;
    }

    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) {
        return lhs.box_pointer != rhs.box_pointer;
    }
};
