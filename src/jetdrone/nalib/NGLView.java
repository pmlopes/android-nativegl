package jetdrone.nalib;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.content.res.AssetManager;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.KeyEvent;

final public class NGLView extends GLSurfaceView implements GLSurfaceView.Renderer, SensorEventListener, NInput {
	
	private static final int BUFFER_MODEL_GROUP = 1;
	private static final int BUFFER_MODEL_INDEX = 2;
	private static final int BUFFER_MODEL_VERTEX = 3;
	private static final int BUFFER_MODEL_NORMAL = 4;
	private static final int BUFFER_MODEL_UV = 5;
	private static final int BUFFER_MODEL_TEX = 6;

	private static final int HAS_NORMALS = 0x0001;
	private static final int HAS_TEXTURE = 0x0002;

	public static final int BUTTON_FIRE = 0x00001;
	public static final int BUTTON_BACK = 0x00002;

	private int nInstance;
	private boolean pause;
	
	private int dx;
	private int dy;
	private int buttons, buttons_buffer;
	private float accel_x, accel_y, accel_z;

	private final AssetManager assetManager;
	
	public NGLView(Context context, AttributeSet attrs) {
		super(context, attrs);
		assetManager = context.getAssets();
		
		nInstance = nCreate(this);
		if (nInstance == 0)
			throw new NullPointerException("NA JNI C NULL");
		setRenderer(this);
	}
	
	@Override
	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		if(!nReset(nInstance, this)) {
			throw new RuntimeException("JNI reset failed");
		}
	}

	@Override
	public void onSurfaceChanged(GL10 gl, int w, int h) {
		if(!nResize(nInstance, w, h)) {
			throw new RuntimeException("JNI resize failed");
		}
	}

	@Override
	public void onDrawFrame(GL10 gl) {
		buttons = buttons_buffer;
		buttons_buffer = 0;
		
		if(!nRender(nInstance, dx, dy, buttons, accel_x, accel_y, accel_z)) {
			throw new RuntimeException("JNI render failed");
		}
	}
	
	@Override
	public void onPause() {
		super.onPause();
		pause = true;
		nPause(nInstance);
	}

	@Override
	public void onResume() {
		nResume(nInstance);
		pause = false;
		super.onResume();
	}
	
	public void onDestroy() {
		nDestroy(nInstance);
		nInstance = 0;
	}
	
	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// ignored for now
	}

	@Override
	public void onSensorChanged(SensorEvent event) {
		if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
			updateAccel(event.values);
		}
	}
	
    @Override
    public final boolean onKeyDown(int keyCode, KeyEvent event) {
        // quit application if user presses the back key.
        if (keyCode == KeyEvent.KEYCODE_BACK) {
        	return updateButtons(BUTTON_BACK);
        }
        if (keyCode == KeyEvent.KEYCODE_MENU) {
        	return updateButtons(BUTTON_FIRE);
        }
        return false;
    }
    
	@Override
	public void updateAccel(float[] values) {
		if(values != null) {
			accel_x = values[0];
			accel_y = values[1];
			accel_z = values[2];
		}
	}
	
	@Override
	public boolean updateAnalog(int dx, int dy) {
		if(pause) return false;
		
		this.dx = dx;
		this.dy = dy;
		return true;
	}
	
	@Override
	public boolean updateButtons(int mask) {
		if(pause) return false;
		
		buttons_buffer |= mask;
		return true;
	}
	
	public void callback(int id) {
		
	}
	
	public void loadModel(int nModelRef, String fileName) throws IOException {
		InputStream in = assetManager.open(fileName);
		if(ByteOrder.nativeOrder() == ByteOrder.LITTLE_ENDIAN) {
			short header = NIO.readShortLE(in);
			if(header == 0x1ee7) {
				short flag = NIO.readShortLE(in);
				boolean hasNormals = (flag & HAS_NORMALS) != 0;
				boolean hasTexture = (flag & HAS_TEXTURE) != 0;
				
				int bufLen;
				
				// read group data
				bufLen = NIO.readIntLE(in);
				NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_GROUP, 4, bufLen, 2));
				
				// read index data
				bufLen = NIO.readIntLE(in);
				NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_INDEX, 2, bufLen, 1));
				
				// buffer length (common to all buffers since we have an indexed model
				bufLen = NIO.readIntLE(in);
				// read vertex data
				NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_VERTEX, 4, bufLen, 3));
				// normal data
				if(hasNormals) {
					NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_NORMAL, 4, bufLen, 3));
				}
				if(hasTexture) {
					// uv data
					NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_UV, 4, bufLen, 2));
					// pixel data
					NIO.readBufferLE(in, nAlloc(nModelRef, BUFFER_MODEL_TEX, 2, NIO.readIntLE(in), NIO.readIntLE(in)));
				}
			}
		}
		in.close();
	}
	
	private static final native ByteBuffer nAlloc(int nModelRef, int type, int size, int width, int height);
	
	private static final native int nCreate(NGLView cb);

	private static final native void nDestroy(int nInstance);

	private static final native boolean nReset(int nInstance, NGLView cb);

	private static final native boolean nResize(int nInstance, int w, int h);

	private static final native boolean nRender(int nInstance, int dx, int dy, int buttons, float accel_x, float accel_y, float accel_z);

	private static final native boolean nPause(int nInstance);

	private static final native boolean nResume(int nInstance);
}
