package jetdrone.util.viewer;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.FileInputStream;
import java.io.IOException;

import javax.media.opengl.DebugGL;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;
import javax.swing.JFrame;

import jetdrone.obj.Model;

import com.sun.opengl.util.Animator;

@SuppressWarnings("serial")
public class JOGLViewer extends JFrame implements GLEventListener, KeyListener {

	/* Model related */
	final Model model;
	
	private static float t=0;
	private static float deltaZ=0;
	
	public JOGLViewer() throws IOException {
		
		model = new Model(new FileInputStream("banana_index.bin"));

		setLocation(900, 750);
		setSize(320, 240);
		setTitle("Jogl Viewer");
		
		GLCanvas canvas = new GLCanvas(new GLCapabilities());
	    canvas.addGLEventListener(this);
	    getContentPane().add(canvas);
	    
	    //Create an Animator linked to the Canvas
	    Animator animator = new Animator(canvas);
	    animator.start();
	    
	    addKeyListener(this);
	}

	/**
	 * Sets up the screen.
	 * 
	 * @see javax.media.opengl.GLEventListener#init(javax.media.opengl.GLAutoDrawable)
	 */
	public void init(GLAutoDrawable drawable) {
		drawable.setGL(new DebugGL(drawable.getGL()));
		final GL gl = drawable.getGL();

		gl.glShadeModel(GL.GL_SMOOTH);
		gl.glClearColor(1, 1, 1, 1);
		
		gl.glEnable(GL.GL_TEXTURE_2D);
		model.loadTexture(gl);
	}

	/**
	 * The only method that you should implement by yourself.
	 * 
	 * @see javax.media.opengl.GLEventListener#display(javax.media.opengl.GLAutoDrawable)
	 */
	public void display(GLAutoDrawable drawable) {

		final GL gl = drawable.getGL();
		final GLU glu = new GLU();

//		int w = drawable.getWidth();
//		int h = drawable.getHeight();

		gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);

        gl.glLoadIdentity();
        glu.gluLookAt(0,0, 10, 0, 0, 0, 0, 1, 0);

		gl.glRotatef(0.1f*t, 0.5f, 1, 0);
		t++;
		
		gl.glPushMatrix();
		{
			//gl.glColor3f(0.7f,1,0.7f);
			gl.glScalef(10, 10, 10);
			model.drawGroup(gl, true, 0);
		}
		gl.glPopMatrix();
	}

	/**
	 * Resizes the screen.
	 * 
	 * @see javax.media.opengl.GLEventListener#reshape(javax.media.opengl.GLAutoDrawable,
	 *      int, int, int, int)
	 */
	public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
		final GL gl = drawable.getGL();
		final GLU glu = new GLU();
		
        gl.glViewport(0, 0, width, height);
        gl.glMatrixMode(GL.GL_PROJECTION);
        gl.glLoadIdentity();
        glu.gluPerspective(45.0f, (float) width / (float) height, 1.0, 200.0);
        gl.glMatrixMode(GL.GL_MODELVIEW);
        gl.glLoadIdentity();
	}

	/**
	 * Changing devices is not supported.
	 * 
	 * @see javax.media.opengl.GLEventListener#displayChanged(javax.media.opengl.GLAutoDrawable,
	 *      boolean, boolean)
	 */
	public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged) {
		throw new UnsupportedOperationException("Changing display is not supported.");
	}

	public void keyPressed(KeyEvent e) {
		if(e.getKeyCode()==KeyEvent.VK_A){
			deltaZ+=0.05;
		}
		if(e.getKeyCode()==KeyEvent.VK_B){
			deltaZ-=0.05;
		}
	}

	public void keyReleased(KeyEvent e) {
	}

	public void keyTyped(KeyEvent e) {
	}

	/**
	 * Starts the JOGL viewer.
	 * 
	 * @param args
	 *            Command line args.
	 */
	public final static void main(String[] args) throws IOException {
		JOGLViewer viewer = new JOGLViewer();
		viewer.setDefaultCloseOperation(EXIT_ON_CLOSE);
		viewer.setVisible(true);
	}
}
