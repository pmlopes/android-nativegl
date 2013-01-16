package jetdrone.nalib;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Point;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

final public class NJoystickView extends View implements OnTouchListener {
	
	public static final int BUTTON_FIRE = 0x00001;
	public static final int BUTTON_BACK = 0x00002;

	private int cx;
	private int cy;
	private Point touchingPoint;
	private boolean dragging = false;

	private final Bitmap joystick;
	private final Bitmap joystickBg;

	private final int offset_x;
	private final int offset_y;
	
	private NInput input;

	public NJoystickView(Context context, AttributeSet attrs) {
		super(context, attrs);
		joystick = BitmapFactory.decodeResource(context.getResources(), R.drawable.joystick);
		joystickBg = BitmapFactory.decodeResource(context.getResources(), R.drawable.joystick_bg);
		
		offset_x = joystick.getWidth() / 2;
		offset_y = joystick.getHeight() / 2;

		setOnTouchListener(this);
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		boolean result = false;

		if (update(event)) {
			result = input.updateAnalog(touchingPoint.x - cx, touchingPoint.y - cy);
			postInvalidate();
		}

		return result;
	}

	@Override
	public void onDraw(Canvas canvas) {
		// init (only executed once)
		if (touchingPoint == null) {
			cx = joystick.getWidth() + joystickBg.getWidth() / 2;
			cy = getHeight() - joystick.getHeight() - joystickBg.getHeight() / 2;
			touchingPoint = new Point(cx, cy);
		}

		// draw the joystick background
		canvas.drawBitmap(joystickBg,
				cx - joystickBg.getWidth() / 2,
				cy - joystickBg.getHeight() / 2,
				null);
		// draw the dragable joystick
		canvas.drawBitmap(joystick,
				touchingPoint.x - joystick.getWidth() / 2,
				touchingPoint.y - joystick.getHeight() / 2,
				null);
	}

	private boolean update(MotionEvent event) {
		
		final int x = (int) event.getX();
		final int y = (int) event.getY();
		
		// drag drop
		if (event.getAction() == MotionEvent.ACTION_DOWN) {
			// if a request is out bound to a box return (not interested)
			if (x < cx - 4 * offset_x) {
				return false;
			}
			if (x > cx + 4 * offset_x) {
				return false;
			}
			if (y < cy - 4 * offset_y) {
				return false;
			}
			if (y > cy + 4 * offset_y) {
				return false;
			}

			dragging = true;
		} else if (event.getAction() == MotionEvent.ACTION_UP) {
			dragging = false;
		}

		int oldx = touchingPoint.x;
		int oldy = touchingPoint.y;

		if (dragging) {
			// get the pos
			touchingPoint.x = x;
			touchingPoint.y = y;

			// bound to a box
			if (touchingPoint.x < cx - offset_x) {
				touchingPoint.x = cx - offset_x;
			}
			if (touchingPoint.x > cx + offset_x) {
				touchingPoint.x = cx + offset_x;
			}
			if (touchingPoint.y < cy - offset_y) {
				touchingPoint.y = cy - offset_y;
			}
			if (touchingPoint.y > cy + offset_y) {
				touchingPoint.y = cy + offset_y;
			}
		} else if (!dragging) {
			// Snap back to center when the joystick is released
			touchingPoint.x = cx;
			touchingPoint.y = cy;
		}

		if (oldx != touchingPoint.x || oldy != touchingPoint.y) {
			return true;
		}
		return false;
	}

    @Override
    public final boolean onKeyDown(int keyCode, KeyEvent event) {
        // quit application if user presses the back key.
        if (keyCode == KeyEvent.KEYCODE_BACK) {
        	return input.updateButtons(BUTTON_BACK);
        }
        if (keyCode == KeyEvent.KEYCODE_MENU) {
        	return input.updateButtons(BUTTON_FIRE);
        }
        return false;
    }
    
	public void setNInput(NInput input) {
		this.input = input;
	}
}
