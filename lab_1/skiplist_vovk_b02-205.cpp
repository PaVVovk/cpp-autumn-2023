#include <iostream>
#include <vector>
#include <random>

using std::endl;
using std::cout;
using std::cin;

template <typename T> class Skiplist{
    size_t s = 0;
    size_t max_h = 0;
    const double P = 0.5; //вероятность добавления элемента

    double rand(double lower = 0.0, double upper = 1.0){
        std::random_device rand_dev;
        std::mt19937 generator(rand_dev());
        std::uniform_real_distribution<double> distr(lower, upper);
        return distr(generator);
    }

    struct elem{
        T value;
        size_t h = 0;
        elem** levels = nullptr;
        elem* prev = nullptr; //нужно для bidirectional iterator, удобно для копирования
    };

    std::vector<elem*> head;
    elem* last = nullptr; //нужно для reverse iterator

public:
    class iterator;
    class reverse_iterator;
    Skiplist(){}

    bool empty(){
        return (s == 0);
    }

    size_t size() {return s;}

    size_t max_height() {return max_h;}

    iterator begin(){ return (s == 0) ? nullptr : head[0]; }
    iterator end(){ return nullptr; }

    class iterator{
    private:
        friend Skiplist<T>;
        elem* curr = nullptr;
    public:
        iterator(){}
        iterator(elem* pointer): curr(pointer){}

        iterator operator++ (int) { iterator tmp = *(this); curr = curr->levels[0]; return tmp; }
        iterator operator-- (int) { iterator tmp = *(this); curr = curr->prev; return tmp; }
        iterator& operator++ () { curr = curr->levels[0]; return *this; }
        iterator& operator-- () { curr = curr->prev; return *this; }

        bool operator== (const iterator& it) { return it.curr == curr; }
        bool operator!= (const iterator& it) { return it.curr != curr; }
        
        T& operator* () { return curr->value; }
        T* operator-> () { T* tmp = &(curr->value); return tmp; }
    };

    reverse_iterator rbegin(){ return (s == 0) ? nullptr : last; }
    reverse_iterator rend(){ return nullptr; }

    class reverse_iterator{
    private:
        friend Skiplist<T>;
        elem* curr = nullptr;
    public:
        reverse_iterator(){}
        reverse_iterator(elem* pointer): curr(pointer){}
    
        reverse_iterator operator++ (int) { reverse_iterator tmp = *(this); curr = curr->prev; return tmp; }
        reverse_iterator operator-- (int) { reverse_iterator tmp = *(this); curr = curr->levels[0]; return tmp; }
        reverse_iterator& operator++ () { curr = curr->prev; return *this; }
        reverse_iterator& operator-- () { curr = curr->levels[0]; return *this; }

        bool operator== (const reverse_iterator& it) { return it.curr == curr; }
        bool operator!= (const reverse_iterator& it) { return it.curr != curr; }
        
        T& operator* () { return curr->value; }
        T* operator-> () { T* tmp = &(curr->value); return tmp; }
    };

    void insert(const T value){
        
        elem* ins_x = new elem;
        ins_x->value = value;
            
        double p = 0.0;
        size_t height = 0;
        do {
            height++;
            p = rand();
        } while ((p >= P) and (max_h >= height));   
        ins_x->levels = new elem*[height];
        ins_x->h = height;
        
        if (s == 0){
            
            for (size_t i = 0; i < height; i++){
                ins_x->levels[i] = nullptr;
            }
            last = ins_x;
            ins_x->prev = nullptr;
            
            max_h = height;
            head.resize(height);
            for (size_t i = 0; i < height; i++){
                head[i] = ins_x;
            }

        } else {

            elem* curr = nullptr;
            elem* next = nullptr;
            for(size_t i = max_h; i > 0; i--){
                if (curr != nullptr){
                    next = curr->levels[i-1];
                } else {
                    next = head[i-1];
                }
                
                while ((next != nullptr) and (next->value < value)){
                    curr = next;
                    next = curr->levels[i-1];
                }
                
                if (i <= height){
                    ins_x->levels[i-1] = next;
                    if (curr == nullptr){
                        head[i-1] = ins_x;
                    } else {
                        curr->levels[i-1] = ins_x;
                    }
                }
            }

            ins_x->prev = curr;
            if (next != nullptr){
                next->prev = ins_x;
            } else {
                last = ins_x;
            }

            if (max_h < height){
                head.resize(height);
                for(size_t i = max_h; i < height; i++){
                    head[i] = ins_x;
                    ins_x->levels[i] = nullptr;
                }
                max_h = height;
            }
        }
        s++;
    }
    
/*
    void insert(auto it_begin, auto it_end){ //вставка по диапазону итератора
        for (auto it = it_begin; it != it_end; it++){
            insert(*(it));
        }
    }

    Skiplist(auto it_begin, auto it_end){ insert(it_begin, it_end); } //конструктор от диапазона итератора
*/

    Skiplist(Skiplist<T> const& list): Skiplist(){
    if (list.s != 0){
        max_h = list.max_h;
        s = list.s;
        head.resize(max_h);
        
        //копирование первого элемента
        elem* curr = list.head[0];
        elem* ins = new elem;
        ins->h = curr->h;
        ins->value = curr->value;
        ins->prev = curr->prev;
        ins->levels = new elem*[ins->h];
        
        for (size_t i = 0; i < ins->h; i++){
            head[i] = ins;
            ins->levels[i] = nullptr;
        }
        
        //копирование и вставка в конец следующих по порядку элементов
        elem* tmp = nullptr;
        size_t c_h = 0;
        while (curr->levels[0] != nullptr){
            tmp = ins;
            curr = curr->levels[0];
            ins = new elem;
            c_h = 0;
            ins->h = curr->h;
            ins->value = curr->value;
            ins->prev = curr->prev;
            ins->levels = new elem*[ins->h];
            for (size_t i = 0; i < ins->h; i++){
                ins->levels[i] = nullptr;
            }
            
            while ((c_h < ins->h) and (tmp != nullptr)){
                for (; c_h < std::min(ins->h, tmp->h); c_h++){
                    tmp->levels[c_h] = ins;
                }
                tmp = tmp->prev;
            }

            if (c_h < ins->h){
                for (size_t i = c_h; i < ins->h; i++){
                    head[i] = ins;
                }
            }
        }
        last = ins;
    }
    }

    Skiplist& operator=(Skiplist<T> const& list){
        Skiplist<T> tmp(list);
        std::swap(this->head, tmp.head);
        std::swap(this->max_h, tmp.max_h);
        std::swap(this->s, tmp.s);
        std::swap(this->last, tmp.last);
        return *this;
    }

    Skiplist(Skiplist<T> &&list): head(list.head), max_h(list.max_h), s(list.s), last(list.last){
        list.s = 0;
        list.max_h = 0;
        list.head.clear();
        list.last = nullptr;

    }

    Skiplist& operator=(Skiplist<T> &&list){
        Skiplist<T> tmp(std::move(list));
        std::swap(this->head, tmp.head);
        std::swap(this->max_h, tmp.max_h);
        std::swap(this->s, tmp.s);
        std::swap(this->last, tmp.last);
        return *this;
    }

    iterator lower_bound(const T value){
        elem* curr = nullptr;
        elem* next = nullptr;
        for (size_t i = max_h; i > 0; i--){
            if (curr == nullptr){
                next = head[i-1];
            } else {
                next = curr->levels[i-1];
            }
            while ((next != nullptr) and (next->value < value)){
                curr = next;
                next = curr->levels[i-1];
            }
        }
        iterator it(curr->levels[0]);
        return it;
    }

    iterator upper_bound(const T value){
        elem* curr = nullptr;
        elem* next = nullptr;
        for (size_t i = max_h; i > 0; i--){
            if (curr == nullptr){
                next = head[i-1];
            } else {
                next = curr->levels[i-1];
            }
            while ((next != nullptr) and (next->value <= value)){
                curr = next;
                next = curr->levels[i-1];
            }
        }
        iterator it(curr->levels[0]);
        return it;
    }

    iterator find(const T value){
        iterator it = lower_bound(value);
        iterator nil(nullptr);
        if ((it == nil) or (*(it) != value)){
            return nil;
        }
        return it;
    }

    size_t count(const T value){
        size_t c = 0;
        iterator start = find(value);
        while ((start != (*this).end()) and (*(start) == value)){
            c++;
            start++;
        }
        return c;
    }

    void clear(){
        if (s != 0){
            elem* curr = head[0];
            elem* next = nullptr;
            while(curr != nullptr){
                next = curr->levels[0];
                delete [] curr->levels;
                delete curr;
                curr = next;
            }
        }
        s = 0;
        max_h = 0;
        last = nullptr;
        head.clear();
    }

    void erase(const Skiplist<T>::iterator it){
        s--;
        size_t i = max_h;
        if (it.curr->h == max_h){
            while (i > 0){
                if ((head[i-1] != it.curr) or (it.curr->levels[i-1] != nullptr)){
                    max_h = i;
                    break;
                }
                i--;
            }
        }
        head.resize(max_h);

        elem* next = it.curr->levels[0];
        if (next == nullptr){
            last = it.curr->prev;
        } else {
            next->prev = it.curr->prev;
        }

        elem* curr = nullptr;
        next = nullptr;
        for (size_t i = max_h; i > 0; i--){
            if (curr == nullptr){
                next = head[i-1];
            } else {
                next = curr->levels[i-1];
            }
            while ((next != nullptr) and (next != it.curr)){
                curr = next;
                next = curr->levels[i-1];
            }

            if (next == it.curr){ 
                if (curr != nullptr){
                    curr->levels[i-1] = next->levels[i-1];
                } else {
                    head[i-1] = next->levels[i-1];
                }
            } else {
                curr = nullptr;
            }
        }

        delete [] it.curr->levels;
        delete it.curr;
    }

    void erase(const Skiplist<T>::iterator it_begin, const Skiplist<T>::iterator it_end){
        for (auto it = it_begin; it != it_end; it++){
            erase(it);
        }
    }

    iterator* equal_range(const T value){  //передаёт массив из 2 итераторов - начала посл-ти и первого эл-та после её конца
        iterator* range = new iterator[2];
        iterator it = find(value);
        range[0] = it;
        while ((it.curr != nullptr) and (*(it) == value)){
            it++;
        }
        range[1] = it;
        return range;
    }

    ~Skiplist(){
        if (s != 0){
            elem* curr = head[0];
            elem* next = nullptr;
            while(curr != nullptr){
                next = curr->levels[0];
                delete [] curr->levels;
                delete curr;
                curr = next;
            }
        }
    }

};

int random(int lower, int upper){
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<> distr(lower, upper);
    return distr(generator);
}

int main(){
    Skiplist<int> A;
    cout << A.empty() << endl;
    A.insert(43);
    A.insert(43);
    for (int i = 0; i < 20; i++){
        A.insert(43);
        A.insert(random(0, 100));
    }
    cout << A.empty() << endl;
    cout << A.size() << endl;
    cout << A.max_height() << endl;
    using It = Skiplist<int>::iterator;
    using RIt = Skiplist<int>::reverse_iterator;
    It it_st = A.begin();
    It it_fin = A.end();
    for (auto it = it_st; it != it_fin; it++){
        cout << *(it) << endl;
    }
    cout << A.count(43) << endl;
    It* x = A.equal_range(43);
    for (auto it = x[0]; it != x[1]; it++){
        cout << *(it) << endl;
    }
    delete [] x;
    return 0;
}