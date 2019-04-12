package Structure;

import java.util.ArrayList;

public class SubNumList {
    public ArrayList<NumList> numLists;
    public SubNumList(int attriNum){
        numLists = new ArrayList<>();
        for( int i = 0; i < attriNum; i++ ) numLists.add(new NumList( i ));
    }
    public static int compare(NumList o1, NumList o2) {
        if ( o1.subNum > o2.subNum ) {
            return 1;
        } else {
            return -1;
        }
    }
    public class NumList{
        public int Attri_id;
        public int subNum;
        public NumList( int i ){
            this.Attri_id = i;
            subNum = 0;
        }
    }
}
