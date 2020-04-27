package Structure;

public class List {
	public double val;
	public int Id;
	//public int PivotId;
	public List(int NumId, double value) {
		this.Id = NumId;
		this.val = value;
	}
	public List(int NumId) {
		this.Id = NumId;
	}
	/*
	public List(int NumId, int pivotId, double value){
		this.PivotId = pivotId;
		this.Id = NumId;
		this.val = value;
	}
	*/
}
