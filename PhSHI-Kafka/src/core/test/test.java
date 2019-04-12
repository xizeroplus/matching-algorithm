package core;

import Structure.EventVal;
import Structure.SubSet;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Semaphore;

public class test {
    private static int n = 0;
    public static final CountDownLatch latch = new CountDownLatch(5);
    public static void main(String[] args){
        /*
        SubSet subSet = new SubSet();
        subSet.add(1,"this");
        subSet.subset.get(0).un_matched_num--;
        System.out.println(subSet.subset.get(0).un_matched_num);
        */
        EventVal v = new EventVal(20,1);
        v.EventStartSendTime = 10;
        long i = v.EventStartSendTime;
        Thread thread = new Thread(() -> {
            try {
                Thread.sleep(3000);
                System.out.println(i);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        });
        thread.start();
        v.EventStartSendTime = 100;
    }
    static class MyThread extends Thread {
        private volatile Semaphore sem;
        private int count;
        MyThread(Semaphore sem, int c) {
         this.sem = sem;
         count = c;
        }
        public void run() {
            /*try{
                //Thread.sleep(1000);
                System.out.println("请求许可 " + count);
                sem.acquire(2);
                System.out.println("许可获得 " + count);
                for(int i = 0; i < 1000; i++)
                    n++;
            }catch (InterruptedException e){
                e.printStackTrace();
            }finally {
                System.out.println("释放许可 " + count);
                sem.release(2);
                latch.countDown();
            }*/
            System.out.println("请求许可 " + count);
            System.out.println("许可获得 " + count);
            n++;
            System.out.println("释放许可 " + count);
            latch.countDown();
        }
    }
}
