package Client;

import Structure.EventVal;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Random;
import java.util.Scanner;

public class creatEvent {
    public static void main(String[] args) {
        Scanner input = new Scanner(System.in);
        BufferedWriter bw = null;
        try{
            File file = new File("resources/eventData.txt");
            FileWriter fw = new FileWriter(file, true);
            bw = new BufferedWriter(fw);
        }catch (Throwable e){
            System.err.println("file write failed!");
            System.exit(1);
        }
        System.out.println("input event num: ");
        int num = input.nextInt();
        input.nextLine();
        System.out.println("input event attribute num: ");
        int attribute_num = input.nextInt();
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
        input.close();
        try {
            bw.flush();
            bw.close();
        }catch (Exception e){
            System.exit(1);
        }
    }
}
