#ifndef OPS_COUNTER_HPP
#define OPS_COUNTER_HPP

#include <stdlib.h>
#include <string.h>
#include <vector>
#include <unordered_map>

template<class T, class R>
using wider_t = typename std::conditional_t<(sizeof(T) > sizeof(R)), T, R>;

class CounterSingle;

template<class, size_t>
class INTCounter;

template<class, size_t>
class PTRCounter;

class Counter {
public:
    enum : size_t {
        BYTE, INT, M256i,
        ADD, SUB, MUL, DIV,
        ASSIGN,
        COMP, BYTEOPS
    };
    void reset();
    size_t getCount(size_t _type, size_t _ops_type);
    void addCount(size_t _type, size_t _ops_type);
    //~Counter() { reset(); }
};

class CounterSingle {

    friend class Counter;

    template<class, size_t> friend class INTCounter;
    template<class, size_t> friend class PTRCounter;

    CounterSingle() {}
    static CounterSingle* instance;

    std::unordered_map<size_t, size_t> int_c;
    std::unordered_map<size_t, size_t> byte_c;
    std::unordered_map<size_t, size_t> m256i_c;

    static void reset() {
        instance->int_c = std::unordered_map<size_t, size_t>();
        instance->byte_c = std::unordered_map<size_t, size_t>();
        instance->m256i_c = std::unordered_map<size_t, size_t>();
    }

    static size_t getCount(size_t _type, size_t _ops_type) {
        switch(_type) {
            case Counter::BYTE:
            {
                if(instance->byte_c.count(_ops_type)) return instance->byte_c[_ops_type];
                else return 0;
                
            }             
            case Counter::INT:
            {
                if(instance->int_c.count(_ops_type)) return instance->int_c[_ops_type];
                else return 0;
            }             
            case Counter::M256i:
            {
                if(instance->m256i_c.count(_ops_type)) return instance->m256i_c[_ops_type];
                else return 0;
            }            
        }
        return 0;
    }

    static void addCount(size_t _type, size_t _ops_type) {
        switch(_type) {
            case Counter::BYTE:
                instance->byte_c[_ops_type] += 1;
            break;
            case Counter::INT:
                instance->int_c[_ops_type] += 1;
            break;
            case Counter::M256i:
                instance->m256i_c[_ops_type] += 1;
            break;
        }
    }

public:
    CounterSingle(const CounterSingle&) = delete;
    CounterSingle(CounterSingle&&) = delete;
    CounterSingle& operator=(const CounterSingle&) = delete;
    CounterSingle& operator=(CounterSingle&&) = delete;
};

//----------------------------------------------------------------
CounterSingle* CounterSingle::instance = new CounterSingle();

void Counter::reset() {
    CounterSingle::reset();
}

size_t Counter::getCount(size_t _type, size_t _ops_type) {
    return CounterSingle::getCount(_type, _ops_type);
}

void Counter::addCount(size_t _type, size_t _ops_type) {
    return CounterSingle::addCount(_type, _ops_type);
}
//----------------------------------------------------------------

template<class INT, size_t TYPE>
class INTCounter {


public:
    template<class, size_t> friend class INTCounter;

    INT value = 0;

    constexpr INTCounter(INT _val) : value(_val) {}
    INTCounter() {};

    template<class INTT, size_t TYPET>
    INTCounter(const INTCounter<INTT, TYPET>& _other){ value = static_cast<INT>(_other.value); };

    template<class INTT, size_t TYPET>
    INTCounter(INTCounter<INTT, TYPET>&& _other){ value = static_cast<INT>(_other.value); };

    //-------------- ASSIGNEMENT --------------

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator=(const INTCounter<INTT, TYPET>& _o) {
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value = static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator=(T _val) {
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value = static_cast<INT>(_val);
        return *this;
    }

    //-------------- MUL --------------

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator*(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::MUL);
        return { static_cast<wider_t<INT, INTT>>(value) * static_cast<wider_t<INT, INTT>>(_o.value) };
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator*(T _val) const{
        CounterSingle::addCount(TYPE, Counter::MUL);
        return { static_cast<wider_t<INT, T>>(value) * static_cast<wider_t<INT, T>>(_val) };
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator*=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::MUL);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value *= static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator*=(T _val) {
        CounterSingle::addCount(TYPE, Counter::MUL);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value *= static_cast<INT>(_val);
        return *this;
    }

    //-------------- ADD --------------

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator+(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::ADD);
        return { static_cast<wider_t<INT, INTT>>(value) + static_cast<wider_t<INT, INTT>>(_o.value) };
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator+(T _val) const{
        CounterSingle::addCount(TYPE, Counter::ADD);
        return INTCounter<wider_t<INT, T>, TYPE>( static_cast<wider_t<INT, T>>(value) + static_cast<wider_t<INT, T>>(_val) );
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator+=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::ADD);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value += static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator+=(T _val) {
        CounterSingle::addCount(TYPE, Counter::ADD);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value += static_cast<INT>(_val);
        return *this;
    }

    //-------------- SUB --------------

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator-(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::SUB);
        return { static_cast<wider_t<INT, INTT>>(value) - static_cast<wider_t<INT, INTT>>(_o.value) };
    }

    INTCounter<INT, TYPE> operator-() const{
        CounterSingle::addCount(TYPE, Counter::SUB);
        return INTCounter<INT, TYPE>(-value);
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator-(T _val) const{
        CounterSingle::addCount(TYPE, Counter::SUB);
        return { static_cast<wider_t<INT, T>>(value) - static_cast<wider_t<INT, T>>(_val) };
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator-=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::SUB);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value -= static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator-=(T _val) {
        CounterSingle::addCount(TYPE, Counter::SUB);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value -= static_cast<INT>(_val);
        return *this;
    }

    //-------------- DIV --------------

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator/(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::DIV);
        return { static_cast<wider_t<INT, INTT>>(value) / static_cast<wider_t<INT, INTT>>(_o.value) };
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator/(T _val) const{
        CounterSingle::addCount(TYPE, Counter::DIV);
        return { static_cast<wider_t<INT, T>>(value) / static_cast<wider_t<INT, T>>(_val) };
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator/=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        CounterSingle::addCount(TYPE, Counter::DIV);
        value /= static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator/=(T _val) {
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        CounterSingle::addCount(TYPE, Counter::DIV);
        value /= static_cast<INT>(_val);
        return *this;
    }

    //-------------- INC --------------

    INTCounter<INT, TYPE>& operator++(){
        CounterSingle::addCount(TYPE, Counter::ADD);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        ++value;
        return *this;
    }

    INTCounter<INT, TYPE>& operator++(int){
        CounterSingle::addCount(TYPE, Counter::ADD);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value++;
        return *this;
    }

    INTCounter<INT, TYPE>& operator--(){
        CounterSingle::addCount(TYPE, Counter::SUB);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        --value;
        return *this;
    }

    INTCounter<INT, TYPE>& operator--(int){
        CounterSingle::addCount(TYPE, Counter::SUB);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value--;
        return *this;
    }

    //-------------- BIT --------------

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator&(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        return { static_cast<wider_t<INT, INTT>>(value) & static_cast<wider_t<INT, INTT>>(_o.value) };
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator&(T _val) const{
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        return { static_cast<wider_t<INT, T>>(value) & static_cast<wider_t<INT, T>>(_val) };
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator&=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value &= static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator&=(T _val) {
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value &= static_cast<INT>(_val);
        return *this;
    }


    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE> operator|(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        return INTCounter<wider_t<INT, INTT>, TYPE>(static_cast<wider_t<INT, INTT>>(value) | static_cast<wider_t<INT, INTT>>(_o.value));
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE> operator|(T _val) const{
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        return { static_cast<wider_t<INT, T>>(value) | static_cast<wider_t<INT, T>>(_val) };
    }

    template<class INTT, size_t TYPET>
    INTCounter<INT, TYPE>& operator|=(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value |= static_cast<INT>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<INT, TYPE>& operator|=(T _val) {
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        CounterSingle::addCount(TYPE, Counter::ASSIGN);
        value |= static_cast<INT>(_val);
        return *this;
    }

    template<class INTT, size_t TYPET>
    INTCounter<wider_t<INT, INTT>, TYPE>& operator<<(const INTCounter<INTT, TYPET>& _o){
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        value = static_cast<wider_t<INT, INTT>>(value) << static_cast<wider_t<INT, INTT>>(_o.value);
        return *this;
    }

    template<class T>
    INTCounter<wider_t<INT, T>, TYPE>& operator<<(T _val) {
        CounterSingle::addCount(TYPE, Counter::BYTEOPS);
        value = static_cast<wider_t<INT, T>>(value) << static_cast<wider_t<INT, T>>(_val);
        return *this;
    }

    //-------------- OTHER --------------

    template<class INTT, size_t TYPET>
    bool operator<(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, INTT>>(value) < static_cast<wider_t<INT, INTT>>(_o.value);
    }

    template<class T>
    bool operator<(T _val) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, T>>(value) < static_cast<wider_t<INT, T>>(_val);
    }

    template<class INTT, size_t TYPET>
    bool operator>(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, INTT>>(value) > static_cast<wider_t<INT, INTT>>(_o.value);
    }

    template<class T>
    bool operator>(T _val) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, T>>(value) > static_cast<wider_t<INT, T>>(_val);
    }

    template<class INTT, size_t TYPET>
    bool operator<=(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, INTT>>(value) <= static_cast<wider_t<INT, INTT>>(_o.value);
    }

    template<class T>
    bool operator<=(T _val) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, T>>(value) <= static_cast<wider_t<INT, T>>(_val);
    }

    template<class INTT, size_t TYPET>
    bool operator>=(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, INTT>>(value) >= static_cast<wider_t<INT, INTT>>(_o.value);
    }

    template<class T>
    bool operator>=(T _val) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, T>>(value) >= static_cast<wider_t<INT, T>>(_val);
    }

    template<class INTT, size_t TYPET>
    bool operator==(const INTCounter<INTT, TYPET>& _o) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, INTT>>(value) == static_cast<wider_t<INT, INTT>>(_o.value);
    }

    template<class T>
    bool operator==(T _val) const{
        CounterSingle::addCount(TYPE, Counter::COMP);
        return static_cast<wider_t<INT, T>>(value) == static_cast<wider_t<INT, T>>(_val);
    }

    template<class T>
    explicit constexpr operator T() const { return T(value); }

};

template<class T, class INT, size_t TYPE, template<typename,size_t> class IT>
INTCounter<wider_t<INT, T>, TYPE> operator+(T a, const IT<INT, TYPE>& b) {
    return b + a;
}

template<class T, class INT, size_t TYPE, template<typename,size_t> class IT>
INTCounter<wider_t<INT, T>, TYPE> operator-(T a, const IT<INT, TYPE>& b) {
    return IT<INT, TYPE>(a) - b;
}

template<class T, class INT, size_t TYPE, template<typename,size_t> class IT>
INTCounter<wider_t<INT, T>, TYPE> operator*(T a, const IT<INT, TYPE>& b) {
    return b * a;
}

template<class T, class INT, size_t TYPE, template<typename,size_t> class IT>
INTCounter<wider_t<INT, T>, TYPE> operator<<(T a, const IT<INT, TYPE>& b) {
    return INTCounter<wider_t<INT, T>, TYPE>(static_cast<wider_t<INT, T>>(a)) << b;
}

#endif  // OPS_COUNTER_HPP
