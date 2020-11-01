package Structure;
public class BitSetVal {
	public boolean state;
	public boolean b;
	public String SubId;
	public BitSetVal() {
		this.state = false;
		this.b = false;
		this.SubId = "";
	}
	public BitSetVal(String s) {
		this.state = true;
		this.b = false;
		this.SubId = s;
	}
}
