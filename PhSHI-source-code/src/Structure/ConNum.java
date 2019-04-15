package Structure;

public class ConNum {
    public int ConSumNum;
    public int[] AttriConNum;
    public ConNum(int attriNum){
        ConSumNum = 0;
        AttriConNum = new int[attriNum];
        for(int i = 0;i < attriNum;i++)
            AttriConNum[i] = 0;
    }
}
