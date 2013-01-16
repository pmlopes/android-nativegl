package jetdrone.convert;

public class Vertex {

	private static short count = 0;
	
	int id;
	
	int v;
	int n;
	int uv;
	
	Vertex(int v, int n, int uv) {
		this.v = v;
		this.n = n;
		this.uv = uv;
	}
	
	void assignId() {
		id = count++;
	}
}
