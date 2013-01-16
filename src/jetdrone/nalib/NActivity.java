package jetdrone.nalib;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

public class NActivity extends Activity {

	private SensorManager sensorManager;
	
	private NGLView nGLView;
	private NJoystickView nJoystickView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// full screen
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		// No title
		requestWindowFeature(Window.FEATURE_NO_TITLE);

		setContentView(R.layout.main);
		
		nGLView = (NGLView) findViewById(R.id.nglview);
		nJoystickView = (NJoystickView) findViewById(R.id.njoystickview);
		
		if(getResources().getBoolean(R.bool.use_sensor)) {
	        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
	        sensorManager.registerListener(nGLView, sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME);
		} else {
			sensorManager = null;
		}
		
		if(nJoystickView != null) {
			nJoystickView.setFocusableInTouchMode(true);
			nGLView.setFocusableInTouchMode(false);
			nJoystickView.setNInput(nGLView);
		} else {
			nGLView.setFocusableInTouchMode(true);			
		}
	}

	@Override
	protected void onPause() {
		if(sensorManager != null)
			sensorManager.unregisterListener(nGLView);
		nGLView.onPause();
		super.onPause();
	}

	@Override
	protected void onResume() {
		if(sensorManager != null)
			sensorManager.registerListener(nGLView, sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_GAME);
		nGLView.onResume();
		super.onResume();
	}

	@Override
	protected void onDestroy() {
		if(sensorManager != null)
			sensorManager.unregisterListener(nGLView);
		nGLView.onDestroy();
		super.onDestroy();
	}
	
	static {
		System.loadLibrary("na");
	}
}
