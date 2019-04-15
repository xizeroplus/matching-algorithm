package Client;

import Structure.SubscribeVal;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.Serializable;
import java.util.*;
import java.text.DecimalFormat;

public class creatZipfSub {
    private static int MaxAttriNum;
    private static ZipfGenerator zipf;
    private static int[] rankMap;
    private static int[] count;

    public static void main(String[] args) {

        int num;
        int rate;

        Scanner input = new Scanner(System.in);
        System.out.println("input sub num:");
        num = input.nextInt();
        input.nextLine();
        System.out.println("input Max Attribute num:");
        MaxAttriNum = input.nextInt();
        input.nextLine();
        System.out.println("input rate n(n/10000):");
        rate = input.nextInt();
        input.nextLine();
        System.out.println("input Alpha:");
        double alpha = input.nextDouble();

        String subFile = "resources/Zipf-sDate-" + num/1000 + "k-a" + MaxAttriNum + "-r" + rate + "-al" + alpha + ".txt";
        String[] name = { "Client1","Client2","Client3" };
        int attribute_num, stock_id;
        Random r = new Random();
        boolean[] a = new boolean[MaxAttriNum];
        zipf = new ZipfGenerator(MaxAttriNum, alpha);
        rankMap = new int[MaxAttriNum];
	count = new int[MaxAttriNum];
        for(int x = 0; x < MaxAttriNum; x++)a[x] = false;
        String s = num + " " + MaxAttriNum + " " + rate;
        for(int i=0;i<MaxAttriNum;i++){
            rankMap[i] = getCount(r, a);
            count[i] = 0;
        }
        //initialize filestream
        BufferedWriter bw = null;
        try{
            File file = new File(subFile);
            FileWriter fw = new FileWriter(file, true);
            bw = new BufferedWriter(fw);
        }catch (Throwable e){
            System.err.println("file write failed");
            System.exit(1);
        }
        try {
            bw.write(s + "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }
        ConsoleProgressBar bar = new ConsoleProgressBar(0,num,100,'*');
        for (int i = 0; i < num; i++) {
            //stock_id = r.nextInt(100);
            stock_id = 1;
            attribute_num = r.nextInt(10) + 1;
            SubscribeVal sVal;
            sVal = new SubscribeVal(attribute_num);
            sVal.SubId = name[r.nextInt(3)];
            sVal.StockId = stock_id;

            for(int x = 0; x < MaxAttriNum; x++)a[x] = false;
            //creat sub
            if (r.nextInt(10000) < rate) {//matched sub
                for(int x = 0; x < attribute_num; x++) {
                    sVal.subVals.get(x).attributeId = getZipfCount(a);
                    count[sVal.subVals.get(x).attributeId]++;
                    sVal.subVals.get(x).min_val = 0.1;
                    sVal.subVals.get(x).max_val = 999.9;
                }
            } else {//unmatched sub
                for(int x = 0; x < attribute_num; x++) {
                    sVal.subVals.get(x).attributeId = getZipfCount(a);
                    count[sVal.subVals.get(x).attributeId]++;
                    if(x == 0){
                        sVal.subVals.get(x).min_val = 0.1;
                        sVal.subVals.get(x).max_val = 0.1;
                        continue;
                    }
                    sVal.subVals.get(x).min_val = r.nextDouble() * 1000;
                    double tmp = r.nextDouble() * 1000;
                    if (tmp >= sVal.subVals.get(x).min_val)
                        sVal.subVals.get(x).max_val = tmp;
                    else {
                        sVal.subVals.get(x).max_val = sVal.subVals.get(x).min_val;
                        sVal.subVals.get(x).min_val = tmp;
                    }
                }
            }
            sVal.subVals.sort(creatZipfSub::compare);
            s =  sVal.SubId + " "
                    + sVal.StockId + " "
                    + sVal.AttributeNum;
            for(int j = 0; j < sVal.AttributeNum; j++){
                s = s + " " + sVal.subVals.get(j).attributeId
                        + " " + sVal.subVals.get(j).min_val
                        + " " + sVal.subVals.get(j).max_val;
            }
            try {
                bw.write(s + "\n");
            } catch (Exception e) {
                e.printStackTrace();
            }
            bar.show((i+1));
        }
        s = "";
        for(int i=0;i<MaxAttriNum;i++) {
            for (Map.Entry<Double, Integer> entry : zipf.map.entrySet()) {
                if (entry.getValue() == i)
                    s = s + "rank:" + i + " AttriId:" + rankMap[i] + " " + entry.getKey() + " ";
            }
        }
        try {
            bw.write(s + "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }
        int tmp = 0;
        s = "";
        for(int i=0;i<MaxAttriNum;i++){
            s = s + count[i] + " ";
            tmp+=count[i];
        }
        s+=tmp;
        try {
            bw.write(s + "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }
        input.close();
        try {
            bw.flush();
            bw.close();
        }catch (Exception e){
            System.exit(1);
        }
    }

    public static class ZipfGenerator implements Serializable {
        private Random random = new Random(0);
        private NavigableMap<Double, Integer> map;
        private static final double Constant = 1.0;

        public ZipfGenerator(int size, double alpha) {
            // create the TreeMap
            map = buildZipf(size, alpha);
        }
        //size为rank个数，alpha为数据倾斜程度, 取值为0表示数据无倾斜，取值越大倾斜程度越高
        private static NavigableMap<Double, Integer> buildZipf(
                int size, double alpha) {
            NavigableMap<Double, Integer> map =
                    new TreeMap<>();
            double div = 0;
            for (int i = 1; i <= size; i++) {
                //the frequency in position i
                div += (Constant / Math.pow(i, alpha));
            }
            double sum = 0;
            for (int i = 1; i <= size; i++) {
                double p = (Constant / Math.pow(i, alpha)) / div;
                sum += p;
                //System.out.print(sum + " ");
                map.put(sum, i - 1);
            }
            return map;
        }

        public int next() {
            double next = random.nextDouble();
            return map.ceilingEntry(next).getValue();
        }
    }

    private static int compare(SubscribeVal.Val o1, SubscribeVal.Val o2) {
        if ( o1.attributeId > o2.attributeId ) {
            return 1;
        } else {
            return -1;
        }
    }
    private static int getZipfCount(boolean[] a) {
        boolean flag = false;
        int t = 0;
        while (!flag) {
            t = rankMap[zipf.next()];
            if (!a[t]) {
                flag = true;
                a[t] = true;
            }
        }
        return t;
    }
    private static int getCount(Random r, boolean[] a) {
        boolean flag = false;
        int t = 0;
        while (!flag) {
            t = r.nextInt(MaxAttriNum);
            if (!a[t]) {
                flag = true;
                a[t] = true;
            }
        }
        return t;
    }
    public static class ConsoleProgressBar {

            private long minimum;

            private long maximum;

            private long barLen;

            private char showChar;

            private DecimalFormat formater = new DecimalFormat("#.##%");

            public ConsoleProgressBar(long minimum, long maximum,
                                      long barLen, char showChar) {
                this.minimum = minimum;
                this.maximum = maximum;
                this.barLen = barLen;
                this.showChar = showChar;
            }

            public void show(long value) {
                if (value < minimum || value > maximum) {
                    return;
                }
                reset();
                minimum = value;
                float rate = (float) (minimum*1.0 / maximum);
                long len = (long) (rate * barLen);
                draw(len, rate);
                if (minimum == maximum) {
                    afterComplete();
                }
            }
            private void draw(long len, float rate) {
                System.out.print("Progress: ");
                for (int i = 0; i < len; i++) {
                    System.out.print(showChar);
                }
                System.out.print(' ');
                System.out.print(format(rate));
            }
            private void reset() {
                System.out.print('\r');
            }
            private void afterComplete() {
                System.out.print('\n');
            }
            private String format(float num) {
                return formater.format(num);
            }
    }
}
