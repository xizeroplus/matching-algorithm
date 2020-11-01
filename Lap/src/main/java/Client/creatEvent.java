package Client;

import Structure.EventVal;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Random;
import java.util.Scanner;

public class creatEvent {
    public static void main(String[] args) {
        if(args.length < 3){
            System.out.println("Usage: creatEvent -resultfile -eventnum -maxattrnum");
            System.exit(1);
        }

        int num = 0, attribute_num = 0;
        String filename = "";
        try{
            filename = args[0];
            num = Integer.parseInt(args[1]);
            attribute_num = Integer.parseInt(args[2]);
        } catch (Throwable e){
            System.out.println("Usage: creatEvent -resultfile -eventnum -maxattrnum");
            e.printStackTrace();
            System.exit(1);
        }
        System.out.println("Config file:" + args[0]);
        System.out.println("event num:" + args[1]);
        System.out.println("attr num:" + args[2]);

//        Scanner input = new Scanner(System.in);
        BufferedWriter bw = null;
        try{
            File file = new File(filename);
            FileWriter fw = new FileWriter(file, true);
            bw = new BufferedWriter(fw);
        }catch (Throwable e){
            System.err.println("file write failed!");
            System.exit(1);
        }

        int stock_id = 1;
        Random r = new Random();
        String s = num + " " + stock_id + " " + attribute_num;
        try {
            bw.write(s + "\n");
        } catch (Exception e) {
            e.printStackTrace();
        }
        for (int i = 0; i < num; i++) {
            EventVal eVal = new EventVal(attribute_num, stock_id);
            eVal.eventVals[0].attributeId = 0;
            eVal.eventVals[0].val = r.nextDouble() * 997 + 2;
            s = eVal.eventVals[0].attributeId + " " + eVal.eventVals[0].val;
            for(int j = 1; j < attribute_num; j++) {
                eVal.eventVals[j].attributeId = j;
                eVal.eventVals[j].val = r.nextDouble() * 997 + 2;
                s = s + " " + eVal.eventVals[j].attributeId + " " + eVal.eventVals[j].val;
            }
            try {
                bw.write(s + "\n");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
//        input.close();
        try {
            bw.flush();
            bw.close();
        }catch (Exception e){
            System.exit(1);
        }
    }
}
