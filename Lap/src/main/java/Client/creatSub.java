package Client;

import Structure.SubscribeVal;

import java.io.*;
import java.text.DecimalFormat;
import java.util.Random;
import java.util.Scanner;

public class creatSub {
    private static int MaxAttriNum;

    public static void main(String[] args) {
        if(args.length < 4){
            System.out.println("Usage: creatSub -resultfile -subnum -maxattrnum -matchrate");
            System.exit(1);
        }
        System.out.println("Config file:" + args[0]);

        int num = 0, rate = 0;
        String subFile = "";
        try{
            subFile = args[0];
            num = Integer.parseInt(args[1]);
            MaxAttriNum = Integer.parseInt(args[2]);
            rate = Integer.parseInt(args[3]);
        } catch (Throwable e){
            System.out.println("Usage: creatSub -resultfile -subnum -maxattrnum -matchrate");
            e.printStackTrace();
            System.exit(1);
        }
//        Scanner input = new Scanner(System.in);
//        System.out.println("input sub num: ");
//        num = input.nextInt();
//        input.nextLine();
//        System.out.println("input Max Attribute num:");
//        MaxAttriNum = input.nextInt();
//        input.nextLine();
//        System.out.println("input rate n(n/10000): ");
//        rate = input.nextInt();

        Random r = new Random();
        BufferedWriter bw = null;
        try{
            File file = new File(subFile);
            FileWriter fw = new FileWriter(file, true);
            bw = new BufferedWriter(fw);
        }catch (Throwable e){
            System.err.println("file write failed!");
            System.exit(1);
        }
        String s = num + " " + MaxAttriNum + " " + rate;
        try {
            bw.write(s + "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }

        ConsoleProgressBar bar = new ConsoleProgressBar(0,num,100,'*');
        boolean[] a = new boolean[MaxAttriNum];
//        String[] name = { "Client1","Client2","Client3" };
        /** subscriber set **/
        String[] name = new String[100000];
        for(int jkl=0; jkl < 100000; jkl++)
            name[jkl] = "Client"+jkl;

        int attribute_num, stock_id;

        for (int i = 0; i < num; i++) {
            //stock_id = r.nextInt(100);
            stock_id = 1;
            attribute_num = r.nextInt(10) + 1;
            SubscribeVal sVal;
            sVal = new SubscribeVal(attribute_num);
            sVal.SubId = name[r.nextInt(100000)];
            sVal.StockId = stock_id;

            for(int x = 0; x < MaxAttriNum; x++)a[x] = false;
            //creat sub
            if (r.nextInt(10000) < rate) {
                //matched sub
                for(int x = 0; x < attribute_num; x++) {
                    sVal.subVals.get(x).attributeId = getCount(r, a);
                    sVal.subVals.get(x).min_val = 0.1;
                    sVal.subVals.get(x).max_val = 999.9;
                }
            } else {
                //unmatched sub
                for(int x = 0; x < attribute_num; x++) {
                    sVal.subVals.get(x).attributeId = getCount(r, a);
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
            sVal.subVals.sort(creatSub::compare);
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
//        input.close();
        try {
            bw.flush();
            bw.close();
        }catch (Exception e){
            System.exit(1);
        }
    }

    private static int compare(SubscribeVal.Val o1, SubscribeVal.Val o2) {
        if ( o1.attributeId > o2.attributeId ) {
            return 1;
        } else {
            return -1;
        }
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
