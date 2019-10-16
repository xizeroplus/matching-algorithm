#include "oldrein.h"


void oldRein::insert(IntervalSub &sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.lowValue;
        c.subID = sub.id;
		//cout<<"ins low "<<i<<endl;
        data[cnt.att][0][c.val / buckStep].push_back(c);
        c.val = cnt.highValue;
		//cout<<"ins high "<<i<<endl;
        data[cnt.att][1][c.val / buckStep].push_back(c);
    }
	++subnum;
}


void oldRein::match(const Pub &pub, int &matchSubs, vector<double> &matchDetailPub)
{

    Timer t;
    vector<bool> bits(subnum, false);
    for (int i = 0; i < pub.size; i++)
    {
        //Timer t0;
        int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
        vector<Combo> &data_0 = data[att][0][buck];

        //Timer t1;
        int data_0_size = data_0.size();
        for (int k = 0; k < data_0_size; k++)
            if (data_0[k].val > value)
                bits[data_0[k].subID] = true;

        //Timer t2;
        for (int j = buck + 1; j < bucks; j++){
            vector<Combo> &data_1 = data[att][0][j];
            int data_1_size = data_1.size();
            for (int k = 0; k < data_1_size; k++)
                bits[data_1[k].subID] = true;
        }

        //Timer t3;
        vector<Combo> &data_2 = data[att][1][buck];
        int data_2_size = data_2.size();
        for (int k = 0; k < data_2_size; k++)
            if (data_2[k].val < value)
                bits[data_2[k].subID] = true;

        //Timer t4;
        for (int j = buck - 1; j >= 0; j--)
        {
            vector<Combo> &data_3 = data[att][1][j];
            int data_3_size = data_3.size();
            for (int k = 0; k < data_3_size; k++)
                bits[data_3[k].subID] = true;
        }

    }

    //Timer t5;
    
    for (int i = 0; i < subnum; i++)
        if (!bits[i])
        {
            matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
            ++matchSubs;
        }

}
