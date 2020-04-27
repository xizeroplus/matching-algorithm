package utils;

import Structure.EventVal;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

public class MatchSendStruct {

    public EventVal eval;
    public Set<String> subSet;

    public MatchSendStruct(EventVal eval) {
        this.eval = eval;
        this.subSet = Collections.synchronizedSet(new HashSet<String>());
    }
}