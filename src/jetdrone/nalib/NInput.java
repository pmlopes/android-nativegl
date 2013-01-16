package jetdrone.nalib;

public interface NInput {

	void updateAccel(float[] values);
	
	boolean updateAnalog(int dx, int dy);
	
	boolean updateButtons(int mask);
}
