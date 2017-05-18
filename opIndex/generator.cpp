#include "generator.h"
#include <cmath>
using namespace std;

void generator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        Sub sub = GenOneSub(i, cons, atts, attDis, valDis, valDom, alpha);
        subList.push_back(sub);
    }
}

void intervalGenerator::GenSubList()
{
    for (int i = 0; i < subs; i++)
    {
        IntervalSub sub = GenOneSub(i, cons, atts, attDis, valDis, valDom, alpha, width);
        subList.push_back(sub);
    }
}

void generator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {
        Pub pub = GenOnePub(m, atts, attDis, valDis, valDom, alpha);
        pubList.push_back(pub);
    }
}

void intervalGenerator::GenPubList()
{
    for (int i = 0; i < pubs; i++)
    {
        Pub pub = GenOnePub(m, atts, attDis, valDis, valDom, alpha);
        pubList.push_back(pub);
    }
}

Sub generator::GenOneSub(int id, int size, int atts, int attDis, int valDis, int valDom, double alpha)
{
    Sub sub;
    sub.id = id;
    sub.size = size;
    if (attDis == 0)
        GenUniformAtts(sub, atts);
    else if (attDis==1)
        GenZipfAtts(sub, atts, alpha);
    if (valDis==0)
        GenUniformValues(sub, valDom);
    return sub;
}

IntervalSub intervalGenerator::GenOneSub(int id, int size, int atts, int attDis, int valDis, int valDom, double alpha, double width)
{
    IntervalSub sub;
    sub.id = id;
    sub.size = size;
    if (attDis == 0)
        GenUniformAtts(sub, atts);
    else if (attDis == 1)
        GenZipfAtts(sub, atts, alpha);
    if (valDis == 0)
        GenUniformValues(sub, valDom);
    return sub;
}

Pub generator::GenOnePub(int m, int atts, int attDis, int valDis, int valDom, double alpha)
{
    Pub pub;
    pub.size = m;
    if (attDis == 0)
        GenUniformAtts(pub,atts);
    else if (attDis == 1)
        GenZipfAtts(pub,atts,alpha);
    if (valDis == 0)
        GenUniformValues(pub,valDom);
    return pub;
}

Pub intervalGenerator::GenOnePub(int m, int atts, int attDis, int valDis, int valDom, double alpha)
{
    Pub pub;
    pub.size = m;
    if (attDis == 0)
        GenUniformAtts(pub,atts);
    else if (attDis == 1)
        GenZipfAtts(pub,atts,alpha);
    if (valDis == 0)
        GenUniformValues(pub,valDom);
    return pub;
}

void generator::GenUniformAtts(Sub &sub, int atts)
{
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenUniformAtts(IntervalSub &sub, int atts)
{
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenUniformAtts(Pub &pub, int atts)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void intervalGenerator::GenUniformAtts(Pub &pub, int atts)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(atts);
        while (CheckExist(a,x))
            x = random(atts);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void generator::GenZipfAtts(Sub &sub, int atts, double alpha)
{
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(atts, alpha);
        while (CheckExist(a,x))
            x = zipfDistribution(atts, alpha);
        a.push_back(x);
        Cnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(IntervalSub &sub, int atts, double alpha)
{
    vector<int> a;
    for (int i = 0; i < sub.size; i++)
    {
        int x = zipfDistribution(atts, alpha);
        while (CheckExist(a,x))
            x = zipfDistribution(atts, alpha);
        a.push_back(x);
        IntervalCnt tmp;
        tmp.att = x;
        sub.constraints.push_back(tmp);
    }
}

void generator::GenZipfAtts(Pub &pub, int atts, double alpha)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(atts, alpha);
        while (CheckExist(a,x))
            x = zipfDistribution(atts, alpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void intervalGenerator::GenZipfAtts(Pub &pub, int atts, double alpha)
{
    vector<int> a;
    for (int i = 0; i < pub.size; i++)
    {
        int x = zipfDistribution(atts, alpha);
        while (CheckExist(a,x))
            x = zipfDistribution(atts, alpha);
        a.push_back(x);
        Pair tmp;
        tmp.att = x;
        pub.pairs.push_back(tmp);
    }
}

void generator::GenUniformValues(Sub &sub, int valDom)
{
    for (int i = 0; i < sub.size; i++)
    {
        int x = random(valDom);
        sub.constraints[i].value = x;
        int y = random(99999);
        sub.constraints[i].op = y%3;
    }
}

void intervalGenerator::GenUniformValues(IntervalSub &sub, int valDom)
{
    for (int i = 0; i < sub.size; i++)
    {
        int x = random( int(valDom * (1.0 - width)) );
        int y = x + int(valDom * width);
        sub.constraints[i].lowValue = x;
        sub.constraints[i].highValue = y;
    }
}

void generator::GenUniformValues(Pub &pub, int valDom)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

void intervalGenerator::GenUniformValues(Pub &pub, int valDom)
{
    for (int i = 0; i < pub.size; i++)
    {
        int x = random(valDom);
        pub.pairs[i].value = x;
    }
}

bool generator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

bool intervalGenerator::CheckExist(vector<int> a,int x)
{
    for (int i = 0; i < a.size(); i++)
        if (a[i] == x)
            return true;
    return false;
}

int generator::zipfDistribution(int n, double alpha)
{
    // alpha > 0
    static bool first = true;
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0;            // Computed exponential value to be returned
    int i;                     // Loop counter

    if (first)
    {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    while (true)
    {
        // Pull a uniform random number (0 < z < 1)
        do
        {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++)
        {
            sum_prob = sum_prob + c / pow((double) i, alpha);
            if (sum_prob >= z)
            {
                zipf_value = i;
                break;
            }
        }

        if (zipf_value >= 1 && zipf_value <= n)
            return int(zipf_value - 1);
    }
}


int intervalGenerator::zipfDistribution(int n, double alpha)
{
    // alpha > 0
    static bool first = true;
    static double c = 0;          // Normalization constant
    double z;                     // Uniform random number (0 < z < 1)
    double sum_prob;              // Sum of probabilities
    double zipf_value = 0;            // Computed exponential value to be returned
    int i;                     // Loop counter

    if (first)
    {
        for (i = 1; i <= n; i++)
            c = c + (1.0 / pow((double) i, alpha));
        c = 1.0 / c;
        first = false;
    }

    while (true)
    {
        // Pull a uniform random number (0 < z < 1)
        do
        {
            z = (double) rand() / RAND_MAX;
        } while ((z == 0) || (z == 1));

        // Map z to the value
        sum_prob = 0;
        for (i = 1; i <= n; i++)
        {
            sum_prob = sum_prob + c / pow((double) i, alpha);
            if (sum_prob >= z)
            {
                zipf_value = i;
                break;
            }
        }

        if (zipf_value >= 1 && zipf_value <= n)
            return int(zipf_value - 1);
    }
}

int generator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}

int intervalGenerator::random(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}