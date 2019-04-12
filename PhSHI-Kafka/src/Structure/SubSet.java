package Structure;

import java.util.concurrent.Semaphore;

public class SubSet {
    public boolean state;
    public Semaphore sem;
    public int attribute_num;
    public int matched_num;
    public int un_matched_num;
    public String SubId;

    public SubSet(int n, String s){
        this.state = true;
        sem = new Semaphore(2);
        this.SubId = s;
        this.attribute_num = n;
        this.matched_num = 0;
        this.un_matched_num = n << 1;
    }
}