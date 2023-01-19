#include <new>
#include <iostream>
#include <cassert>

class C {
public:
    C(int id) : id_{id} {
        std::cout << "Created C. id: " << id_ << std::endl;
    }
    C(const C & other) {
        id_ = other.id_;
        std::cout << "Created C. id: " << id_ << std::endl;
    } 
    ~C() {
        std::cout << "Destructed C. id: " << id_ << std::endl;
    }
private:
    int id_;
};

template <class T>
class UniquePtr {
public:
    template <class... Args>
    UniquePtr(Args&&... args) : exist_{true}, data_{std::forward<Args>(args)...} {}
    ~UniquePtr() { if(exist_) data_.~T(); }
    UniquePtr(UniquePtr & other) = delete;
    UniquePtr(UniquePtr && other) : exist_{other.exist_} {
        if (exist_) {
            new (&data_) T{other.data_};
            other.data_.~T();
            other.exist_ = false;
        }
    }
    UniquePtr() : exist_{false} {}
    UniquePtr & operator=(UniquePtr & other) = delete;
    UniquePtr & operator=(UniquePtr && other) {
        if (exist_) {
            data_.~T();
        }
        exist_ = other.exist_;
        if (exist_) {
            new (&data_) T{other.data_};
            other.data_.~T();
            other.exist_ = false;
        }
        return *this;
    }
    T operator*() {
        assert(exist_);
        return data_;
    }
private:
    bool exist_;
    union {
        // Activated only if parent_ == nullptr and data_exist
        T data_;
    };
};

int main()
{
    UniquePtr<C> s1{42};
    UniquePtr<C> s2{53};
    s2 = std::move(s1);
    return 0;
}