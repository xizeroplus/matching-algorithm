package MySerdes;

import Structure.EventVal;
import Structure.SubscribeVal;

public class ValueSerde {
    static public final class EventSerde extends WrapperSerde<EventVal> {
        public EventSerde() {
            super(new JsonSerializer<>(), new JsonDeserializer<>(EventVal.class));
        }
    }
    static public final class SubscribeSerde extends WrapperSerde<SubscribeVal> {
        public SubscribeSerde() { super(new JsonSerializer<>(), new JsonDeserializer<>(SubscribeVal.class)); }
    }
    static public final class EventValSerde extends JsonSerializer<EventVal>{}
    static public final class EventValDeserde extends JsonDeserializer<EventVal>{
    	public EventValDeserde() { super(EventVal.class); }
    }
    static public final class  SubValSerde extends  JsonSerializer<SubscribeVal>{}
    static  public  final class SubValDeserde extends JsonDeserializer<SubscribeVal>{
        public  SubValDeserde(){ super(SubscribeVal.class); }
    }
}
