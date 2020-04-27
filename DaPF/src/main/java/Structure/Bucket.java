package Structure;

import java.util.ArrayList;

public class Bucket {
	public ArrayList<List> bucket;
	public boolean[] bitSet;
	public int executeNum;
	public Bucket() {
		this.bucket = new ArrayList();
	}
	public Bucket(int n){
		bitSet = new boolean[n];
		executeNum = 0;
		for(int i =0;i < n; i++)
			bitSet[i] = false;
	}
}