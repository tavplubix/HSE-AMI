#include <stdio.h>

class Base
{
private:
    int value = 0;
public:
    virtual ~Base() {}
    virtual void func() const
    {
        printf("Base::func(): %d\n", value);
    }
    void set_value(int value) { this->value = value; }
    int get_value() const { return value; }
    void hijack();
};


class Derived : public Base
{
public:
    virtual ~Derived() {}
    virtual void func() const
    {
        printf("Derived::func(): %d\n", this->get_value());
    }
};

extern "C" void myfunc(void*);

int main() {
    Base *p = new Base;
    Base *q = new Derived;
    p->set_value(5);
    q->set_value(8);
    p->func();
    //myfunc(p);
    //myfunc(q);
    p->hijack();
    p->func();
    p->func();
    q->hijack();
    //q->get_value();
    //q->set_value(1);
    q->func();
    p->func();
    q->func();
    delete p;
    delete q;
}


