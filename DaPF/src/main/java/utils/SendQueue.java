package utils;

import java.util.concurrent.LinkedBlockingQueue;

public class SendQueue {
    private LinkedBlockingQueue<MatchSendStruct> sendqueue;

    public SendQueue(int size){
        sendqueue = new LinkedBlockingQueue<>(10);
    }

}
