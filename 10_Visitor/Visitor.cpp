#include <vector>
#include <iostream>


class BaseVisitor {
public:
    // we need at least one virtual function to allow dynamic_cast
    virtual ~BaseVisitor() {}
};

template <class T, typename R = void>
class Visitor {
public:
    typedef R ReturnType;
    virtual ReturnType Visit(T&) = 0;
};

#define DEFINE_VISITABLE() \
virtual ReturnType Accept(BaseVisitor& guest) \
    { return AcceptImpl(*this, guest); }


template <class R, class Visited>
struct DefaultCatchAll {
    static R OnUnknownVisitor(Visited&, BaseVisitor&) {
        /* here custom default action can be put for
           visiting an unknown type */
        return R();
    }
};

template <typename R = void,
          template <typename, class> class CatchAll = DefaultCatchAll>
class BaseVisitable {
public:
    typedef R ReturnType;
    virtual ~BaseVisitable() {}
    virtual ReturnType Accept(BaseVisitor&) = 0;

protected:
    template<class T>
    static ReturnType AcceptImpl(T& visited, BaseVisitor& guest) {
        if (Visitor<T, R>* p = dynamic_cast<Visitor<T, R>*>(&guest)) {
            return p->Visit(visited);
        }

        return CatchAll<R, T>::OnUnknownVisitor(visited, guest);
    }
};



class DocElement : public BaseVisitable<> {
public:
    DEFINE_VISITABLE()
};

class Paragraph : public DocElement {
public:
    DEFINE_VISITABLE()
};



class MyConcreteVisitor :
    public BaseVisitor,
    public Visitor<DocElement>,
    public Visitor<Paragraph> {

public:
    void Visit(DocElement&) {
        std::cout << "Visit(DocElement&)\n";
    }

    void Visit(Paragraph&) {
        std::cout << "Visit(Paragraph&)\n";
    }
};



int main() {
    MyConcreteVisitor visitor;
    Paragraph par;
    par.Accept(visitor);
    
    DocElement* d = &par;
    d->Accept(visitor);
}
