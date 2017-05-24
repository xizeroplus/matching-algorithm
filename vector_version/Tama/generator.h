#ifndef _GENERATOR_H
#define _GENERATOR_H
#include "data_structure.h"

class generator {
    void GenUniformAtts(Sub &sub, int atts);

    void GenUniformAtts(Pub &pub, int atts);

    void GenZipfAtts(Sub &sub, int atts, double alpha);

    void GenZipfAtts(Pub &pub, int atts, double alpha);

    void GenUniformValues(Sub &sub, int valDom);

    void GenUniformValues(Pub &pub, int valDom);

    bool CheckExist(vector<int> a, int x);

    int random(int x);

    int zipfDistribution(int n, double alpha);

    Sub GenOneSub(int id, int size, int atts, int attDis, int valDis, int valDom, double alpha);

    Pub GenOnePub(int m, int atts, int attDis, int valDis, int valDom, double alpha);

public:
    vector<Sub> subList;
    vector<Pub> pubList;
    int subs, pubs, atts, cons, m, attDis, valDis, valDom;
    double alpha;

    generator(int subs, int pubs, int atts, int cons, int m, int attDis, int valDis, int valDom, double alpha) :
            subs(subs), pubs(pubs), atts(atts), cons(cons), m(m), attDis(attDis), valDis(valDis), valDom(valDom),
            alpha(alpha)
    {}

    void GenSubList();

    void GenPubList();
};


class intervalGenerator {
    void GenUniformAtts(IntervalSub &sub, int atts);

    void GenUniformAtts(Pub &pub, int atts);

    void GenZipfAtts(IntervalSub &sub, int atts, double alpha);

    void GenZipfAtts(Pub &pub, int atts, double alpha);

    void GenUniformValues(IntervalSub &sub, int valDom);

    void GenUniformValues(Pub &pub, int valDom);

    bool CheckExist(vector<int> a, int x);

    int random(int x);

    int zipfDistribution(int n, double alpha);

    IntervalSub GenOneSub(int id, int size, int atts, int attDis, int valDis, int valDom, double alpha, double width);

    Pub GenOnePub(int m, int atts, int attDis, int valDis, int valDom, double alpha);

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
    int subs, pubs, atts, cons, m, attDis, valDis, valDom;
    double alpha, width;

    intervalGenerator(int subs, int pubs, int atts, int cons, int m, int attDis, int valDis, int valDom, double alpha,
                      double width = 0.5) :
            subs(subs), pubs(pubs), atts(atts), cons(cons), m(m), attDis(attDis), valDis(valDis), valDom(valDom),
            alpha(alpha), width(width)
    {}

    void GenSubList();

    void GenPubList();
};

#endif
