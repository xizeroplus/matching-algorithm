package Structure;

import java.io.*;
import java.util.*;

public class SubscribeVal {
	public String SubId;
	public int StockId;
	public int AttributeNum;
	public int Pivot_Attri_Id;
	public ArrayList<Val> subVals;

	public class Val{
		public int attributeId;
		public double max_val;
		public double min_val;

		public Val() {
			attributeId = 0;
			max_val = 0;
			min_val = 0;
		}
	}
	public SubscribeVal(int attributeNum) {
		this.Pivot_Attri_Id = -1;
		this.AttributeNum = attributeNum;
		this.subVals = new ArrayList<>();
		for(int i = 0; i < attributeNum; i++) {
			this.subVals.add(new Val());
		}
	}
}
