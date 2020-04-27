package Structure;

import java.util.ArrayList;

public class Index_Op {
    public int SubNum;
    public boolean Pivot;
    public OpList opList;
    public Index_Op(int op_num, int arrti_num){
        this.Pivot = false;
        this.SubNum = 0;
        this.opList = new OpList(op_num, arrti_num);
    }
    public class OpList{
        public OpBucket[] opBuckets;
        public OpList(int op_num, int arrti_num){
            opBuckets = new OpBucket[op_num];
            for(int i = 0; i < op_num; i++)opBuckets[i] = new OpBucket(arrti_num);
        }
    }
    public class OpBucket{
        public boolean[] bits;
        public OpBucketList[] opBucketLists;
        public OpBucket(int n){
            bits = new boolean[n];
            opBucketLists = new OpBucketList[n];
            for(int i = 0; i < n; i++) {
                bits[i] = false;
                opBucketLists[i] = new OpBucketList();
            }
        }
    }
    public class OpBucketList{
        public ArrayList<List> opBucketList;
        public OpBucketList(){opBucketList = new ArrayList<>();}
    }
}
