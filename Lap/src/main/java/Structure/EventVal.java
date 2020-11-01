package Structure;
public class EventVal {
	public int StockId;
	public int AttributeNum;
	public long EventProduceTime;
	public long EventArriveTime;
	public long EventMatchTime;
	public long EventStartSendTime;
	public long EventGetTime;
	public Val[] eventVals;
	public class Val{
		public int attributeId;
		public double val;
		public Val() {
			attributeId = 0;
			val = 0;
		}
	}
	public EventVal(int attributeNum, int Id) {
		EventProduceTime = 0;
		EventArriveTime = 0;
		EventMatchTime = 0;
		EventStartSendTime = 0;
		EventGetTime = 0;
		this.StockId = Id;
		this.AttributeNum = attributeNum;
		this.eventVals = new Val[attributeNum];
		for(int i = 0; i < attributeNum; i++) {
			this.eventVals[i] = new Val();
		}
	}
}
