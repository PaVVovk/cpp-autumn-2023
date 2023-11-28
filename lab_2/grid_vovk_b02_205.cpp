#include <iostream>
#include <cassert>

using std::endl;
using std::cout;
using std::cin;

//класс 2-мерной сетки
template <typename T>
class Grid final {  
public:
    using value_type = T;  
    using size_type = unsigned; 
private:
    T* data; 
    size_type y_size, x_size;
    //y - строки, x - столбцы

    class GridRow {
        Grid& parent;
        size_type y;
    public:
        GridRow(Grid& parent, size_type(y)): parent(parent), y(y) {}
        T& operator[](size_type x) { return parent(y, x); }
    };

    friend class GridRow;
public:
    Grid() : data(nullptr), y_size(0), x_size(0) { } 

    //доп. конструкторы

    explicit Grid(T const& t) : data(new T[1]), y_size(1), x_size(1) { 
        data[0] = t;
    }

    Grid(size_type y_size, size_type x_size) : data(new T[y_size * x_size]), y_size(y_size), x_size(x_size) { 
        for (size_type i = 0; i < y_size * x_size; i++) {
            data[i] = T();
        }
    }

    Grid(size_type y_size, size_type x_size, T const& t) : data(new T[y_size * x_size]), y_size(y_size), x_size(x_size) { 
        for (size_type i = 0; i < y_size * x_size; i++) {
            data[i] = t;
        }
    }

    //копирование и копирующее присваивание

    Grid(Grid<T> const& other): data(new T[other.y_size * other.x_size]), y_size(other.y_size), x_size(other.x_size) { 
        for (size_type i = 0; i < y_size * x_size; i++) {
            data[i] = other.data[i];
        }
    }

    Grid<T>& operator=(Grid<T> const& other){ 
        Grid<T> tmp(other);
        std::swap(tmp.data, this->data);
        std::swap(tmp.x_size, this->x_size);
        std::swap(tmp.y_size, this->y_size);
        return *this;
    }

    //перемещение и перемещающее присваивание

    Grid(Grid<T>&& other): data(other.data), y_size(other.y_size), x_size(other.x_size) { 
        other.data = nullptr;
        other.y_size = 0;
        other.x_size = 0;
    }

    Grid<T>& operator=(Grid<T>&& other){ 
        Grid<T> tmp(std::move(other));
        std::swap(tmp.data, this->data);
        std::swap(tmp.x_size, this->x_size);
        std::swap(tmp.y_size, this->y_size);
        return *this;
    }

    //деструктор

    ~Grid() {
        if (data != nullptr){
            delete[] data;
        }
    }

    size_type get_y_size() const { return y_size; } 
    size_type get_x_size() const { return x_size; }

    //индексирование с помощью operator()

    T& operator()(size_type y, size_type x) { 
        return data[y * x_size + x];
    }

    //индексирование с помощью [][] (см. вспом. класс выше)

    GridRow operator[](size_type y) { return GridRow(*this, y); }

    void print(){
        for (size_type i = 0; i < y_size; i++){
            for (size_type j = 0; j < x_size; j++){
                cout << data[i * x_size + j] << ' ';
            }
            cout << endl;
        }
    }
};

//n-мерная сетка, заданная рекурсивно
template <typename T, size_t n>
class nGrid{
    public:  
    using size_type = unsigned; 
private:
    nGrid<T, n-1>* data; 
    size_type len;
public:
    nGrid() : data(nullptr), len(0) { } 

    //доп. конструкторы

    template <typename... Args>
    explicit nGrid(size_type len, Args... coords) : data(new nGrid<T,n-1>[len]), len(len) { 
        for (size_type i = 0; i < len; i++){
            nGrid<T,n-1> tmp(coords...);
            data[i] = std::move(tmp);
        }
    }

    template <typename... Args>
    explicit nGrid(size_type len, Args... coords, T const& t) : data(new nGrid<T,n-1>[len]), len(len) { 
        for (size_type i = 0; i < len; i++){
            nGrid<T,n-1> tmp(coords... , t);
            data[i] = std::move(tmp);
        }
    }

    //копирование и копирующее присваивание

    nGrid(nGrid<T,n> const& other): data(new nGrid<T,n-1>[other.len]), len(other.len) { 
        for (size_type i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
    }

    nGrid<T,n>& operator=(nGrid<T,n> const& other){ 
        nGrid<T,n> tmp(other);
        std::swap(tmp.data, this->data);
        std::swap(tmp.len, this->len);
        return *this;
    }

    //перемещение и перемещающее присваивание

    nGrid(nGrid<T,n>&& other): data(other.data), len(other.len){ 
        other.data = nullptr;
        other.len = 0;
    }

    nGrid<T,n>& operator=(nGrid<T,n>&& other){ 
        nGrid<T,n> tmp(std::move(other));
        std::swap(tmp.data, this->data);
        std::swap(tmp.len, this->len);
        return *this;
    }

    //деструктор

    void clear(){
        if (data != nullptr){
            for (size_type i = 0; i < len; i++){
                data[i].clear();
            }
            delete[] data;
        }
        data = nullptr;
    }

    ~nGrid() {
        clear();
    }

    //индексирование с помощью operator()

    template <typename... Args>
    T& operator()(size_type y, Args... coords) { 
        return data[y](coords...);
    }

    template <typename... Args>
    T operator() (size_type y, Args... coords) const { 
        return data[y](coords...);
    }

    //индексирование с помощью [][]...[]

    nGrid<T,n-1>& operator[](size_type y) { return data[y]; }

    nGrid<T,n-1> operator[](size_type y) const { return data[y]; }

    void print(){
        for (size_type i = 0; i < len; i++){
            data[i].print();
        }
        cout << "------" << endl;
    }
};

//частный случай nGrid - 1-мерная сетка (строка)
template <typename T>
class nGrid<T,1>{
    public:  
    using size_type = unsigned; 
private:
    T* data; 
    size_type len;
public:
    nGrid() : data(nullptr), len(0) { } 

    //доп. конструкторы

    explicit nGrid(size_type len) : data(new T[len]), len(len) { 
        for (size_type i = 0; i < len; i++) {
            data[i] = T();
        }
    }

    nGrid(size_type len, T const& t) : data(new T[len]), len(len) { 
        for (size_type i = 0; i < len; i++) {
            data[i] = t;
        }
    }

    //копирование и копирующее присваивание

    explicit nGrid(nGrid<T,1> const& other): data(new T[other.len]), len(other.len) { 
        for (size_type i = 0; i < len; i++) {
            data[i] = other.data[i];
        }
    }

    nGrid<T,1>& operator=(nGrid<T,1> const& other){ 
        nGrid<T,1> tmp(other);
        std::swap(tmp.data, this->data);
        std::swap(tmp.len, this->len);
        return *this;
    }

    //перемещение и перемещающее присваивание

    explicit nGrid(nGrid<T,1>&& other): data(other.data), len(other.len) { 
        other.data = nullptr;
        other.len = 0;
    }

    nGrid<T,1>& operator=(nGrid<T,1>&& other){ 
        nGrid<T,1> tmp(std::move(other));
        std::swap(tmp.data, this->data);
        std::swap(tmp.len, this->len);
        return *this;
    }

    //деструктор

    void clear(){
        if (data != nullptr){
            delete[] data;
        }
        data = nullptr;
    }

    ~nGrid() {
        clear();
    }

    //индексирование с помощью operator()

    T& operator()(size_type y) { 
        return data[y];
    }

    T operator()(size_type y) const { 
        return data[y];
    }

    //индексирование с помощью []

    T& operator[](size_type y) { return data[y]; }

    T operator[](size_type y) const { return data[y]; }

    void print(){
        for (size_type i = 0; i < len; i++){
            cout << data[i] << ' ';
        }
        cout << endl;
    }
};

int main() {
    //тесты Шевелёва
    Grid<float> g(3, 2, 0.0f);
    assert(3 == g.get_y_size());
    assert(2 == g.get_x_size());
    g.print();

    using gsize_t = typename Grid<float>::size_type;

    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            assert(0.0f == g[y_idx][x_idx]);
    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            g[y_idx][x_idx] = 1.0f;

    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx)
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx)
            assert(1.0f == g(y_idx, x_idx));


    //мои тесты
    float c = 0.0;
    for (gsize_t y_idx = 0; y_idx != g.get_y_size(); ++y_idx){
        for (gsize_t x_idx = 0; x_idx != g.get_x_size(); ++x_idx){
            g[y_idx][x_idx] += c;
            c++;
        }
    }
    g.print();

    //тесты Шевелёва
    nGrid<float, 3> const g3(2, 3, 4, 1.0f);
    assert(1.0f == g3(1, 1, 1));

    nGrid<float, 2> g2(2, 5, 2.0f);
    assert(2.0f == g2(1, 1));

    g2 = g3[1];
    assert(1.0f == g2(1, 1));

    //мои тесты
    c = 0.0f;
    nGrid<float, 3> h(5, 7, 3, 0.0f);
    for (gsize_t x = 0; x < 5; x++){
        for (gsize_t y = 0; y < 7; y++){
            for (gsize_t z = 0; z < 3; z++){
                h[x][y][z] += c;
                c++;
            }
        }
    }
    h.print();
    h[2].print();
    h[2][5].print();
    cout << h[2][5][1] << endl;
    cout << h(2, 5, 1) << endl;
    return 0;
}