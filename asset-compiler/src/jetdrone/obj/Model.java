package jetdrone.obj;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import javax.media.opengl.GL;

public class Model {

	public static final int HAS_NORMALS = 0x0001;
	public static final int HAS_TEXTURE = 0x0002;
	
	/* index begin/end */
	public final int[][] group;
	Buffer[] gidxBuf;
	public final int vertices;
	
	public final Buffer idxBuf;
	public final Buffer vBuf;
	public final Buffer nBuf;
	public final Buffer uvBuf;
	public final Buffer texBuf;
	public final int tw;
	public final int th;
	
	private int texId;

	public void drawGroup(GL gl, boolean bindBuffers, int groupId) {
		if(bindBuffers) {
			gl.glVertexPointer(3, GL.GL_FLOAT, 0, vBuf);
			gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
			if(nBuf != null) {
				gl.glNormalPointer(GL.GL_FLOAT, 0, nBuf);
				gl.glEnableClientState(GL.GL_NORMAL_ARRAY);
			}
			if(uvBuf != null) {
				gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, uvBuf);
				gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);
				gl.glBindTexture(GL.GL_TEXTURE_2D, texId);
			} else {
				gl.glBindTexture(GL.GL_TEXTURE_2D, 0);
			}
		}
		gl.glDrawElements(GL.GL_TRIANGLES, group[groupId][1], GL.GL_UNSIGNED_SHORT, gidxBuf[groupId]);
	}

	public void draw(GL gl) {
		gl.glVertexPointer(3, GL.GL_FLOAT, 0, vBuf);
		gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
		if(nBuf != null) {
			gl.glNormalPointer(GL.GL_FLOAT, 0, nBuf);
			gl.glEnableClientState(GL.GL_NORMAL_ARRAY);
		}
		
		if(uvBuf != null) {
			gl.glTexCoordPointer(2, GL.GL_FLOAT, 0, uvBuf);
			gl.glEnableClientState(GL.GL_TEXTURE_COORD_ARRAY);
			gl.glBindTexture(GL.GL_TEXTURE_2D, texId);
		} else {
			gl.glBindTexture(GL.GL_TEXTURE_2D, 0);
		}
		
		gl.glDrawElements(GL.GL_TRIANGLES, vertices, GL.GL_UNSIGNED_SHORT, idxBuf);
	}

	public void loadTexture(GL gl) {
		if(texBuf != null) {
			int[] texPtr = new int[1];
			
			gl.glGenTextures(1, texPtr, 0);
			texId = texPtr[0];
			gl.glBindTexture(GL.GL_TEXTURE_2D, texId);
			gl.glTexImage2D(GL.GL_TEXTURE_2D, 0, GL.GL_RGB, tw, th, 0, GL.GL_RGB, GL.GL_UNSIGNED_SHORT_5_6_5, texBuf);
			gl.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_LINEAR);
			gl.glTexParameterf(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_LINEAR);
		}
	}
	
	private static final short readShortLE(final InputStream in) throws IOException {
		final byte[] buffer = new byte[2];
		
		if(in.read(buffer) != 2) {
			throw new IOException();
		}
		
		return (short) ((buffer[0] & 0xFF) | (buffer[1] & 0xFF) << 8);
	}
	
	private static final int readIntLE(final InputStream in) throws IOException {
		final byte[] buffer = new byte[4];
		
		if(in.read(buffer) != 4) {
			throw new IOException();
		}
		
		return (buffer[0] & 0xFF) | (buffer[1] & 0xFF) << 8 | (buffer[2] & 0xFF) << 16 | (buffer[3] & 0xFF) << 24;
	}
	
	private static final void readBufferLE(final InputStream in, final Buffer buf) throws IOException {
		final byte[] buffer = new byte[4096];
		final ByteBuffer bb = (ByteBuffer) buf;
		int bufLen = buf.capacity();
		int readBytes = 0;
		
		bb.order(ByteOrder.LITTLE_ENDIAN);
		
		while(bufLen > 0) {
			if(bufLen > buffer.length) {
				readBytes = in.read(buffer, 0, buffer.length);
				bb.put(buffer, 0, buffer.length);
				bufLen -= readBytes;
			} else {
				readBytes = in.read(buffer, 0, bufLen);
				bb.put(buffer, 0, bufLen);
				bufLen -= readBytes;
			}
		}
		buf.rewind();
	}
	
//	public Model(InputStream in) throws IOException {
//		
//		if(ByteOrder.nativeOrder() == ByteOrder.LITTLE_ENDIAN) {
//			short header = readShortLE(in);
//			if(header == 0x1ee7) {
//				short flag = readShortLE(in);
//				boolean hasNormals = (flag & HAS_NORMALS) != 0;
//				boolean hasUVs = (flag & HAS_UVS) != 0;
//				boolean hasPixels = (flag & HAS_PIXELS) != 0;
//				// read group data
//				group = new int[readIntLE(in)][2];
//				for(int i=0; i<group.length; i++) {
//					group[i][0] = readIntLE(in);
//					group[i][1] = readIntLE(in);
//				}
//				int bufLen;
//				// read vertex data
//				bufLen = readIntLE(in);
//				totalFaces = bufLen / 3;
//				vBuf = ByteBuffer.allocateDirect(bufLen * 4);
//				readBufferLE(in, vBuf);
//System.out.println("V: " + vBuf.capacity() / 1024 + "Kb");
//				// normal data
//				if(hasNormals) {
//					bufLen = readIntLE(in);
//					nBuf = ByteBuffer.allocateDirect(bufLen * 4);
//					readBufferLE(in, nBuf);
//System.out.println("N: " + nBuf.capacity() / 1024 + "Kb");
//				} else {
//					nBuf = null;
//				}
//				// uv data
//				if(hasUVs) {
//					bufLen = readIntLE(in);
//					uvBuf = ByteBuffer.allocateDirect(bufLen * 4);
//					readBufferLE(in, uvBuf);
//System.out.println("UV: " + uvBuf.capacity() / 1024 + "Kb");
//				} else {
//					uvBuf = null;
//				}
//				// pixel data
//				if(hasPixels) {
//					tw = readIntLE(in);
//					th = readIntLE(in);
//					bufLen = tw * th;
//					texBuf = ByteBuffer.allocateDirect(bufLen * 2);
//					readBufferLE(in, texBuf);
//System.out.println("P: " + texBuf.capacity() / 1024 + "Kb");
//				} else {
//					tw = 0;
//					th = 0;
//					texBuf = null;
//				}
//				return;
//			}
//			throw new RuntimeException("File Format is not recognized");
//		}
//		throw new RuntimeException("Native Order is not LE");
//	}
	
	public Model(InputStream in) throws IOException {
		
		if(ByteOrder.nativeOrder() == ByteOrder.LITTLE_ENDIAN) {
			short header = readShortLE(in);
			if(header == 0x1ee7) {
				short flag = readShortLE(in);
				boolean hasNormals = (flag & HAS_NORMALS) != 0;
				boolean hasTexture = (flag & HAS_TEXTURE) != 0;
				// read group data
				group = new int[readIntLE(in)][2];
				gidxBuf = new Buffer[group.length];
				for(int i=0; i<group.length; i++) {
					group[i][0] = readIntLE(in);
					group[i][1] = readIntLE(in);
				}
				int bufLen;
				// read index data
				bufLen = readIntLE(in);
				vertices = bufLen;
				idxBuf = ByteBuffer.allocateDirect(bufLen * 2);
				readBufferLE(in, idxBuf);
				System.out.println("IDX: " + idxBuf.capacity() / 1024 + "Kb");
				
				for(int i=0; i<group.length; i++) {
					idxBuf.position(group[i][0] * 2);
					gidxBuf[i] = ((ByteBuffer) idxBuf).asReadOnlyBuffer();
					((ByteBuffer) gidxBuf[i]).order(ByteOrder.LITTLE_ENDIAN);
					gidxBuf[i].limit((group[i][1] - group[i][0]) * 2);
					gidxBuf[i].rewind();
				}
				idxBuf.rewind();
				
				
				// buffer length
				bufLen = readIntLE(in);
				
				// read vertex data
				vBuf = ByteBuffer.allocateDirect(bufLen * 4 * 3);
				readBufferLE(in, vBuf);
				System.out.println("V: " + vBuf.capacity() / 1024 + "Kb");
				// normal data
				if(hasNormals) {
					nBuf = ByteBuffer.allocateDirect(bufLen * 4 * 3);
					readBufferLE(in, nBuf);
					System.out.println("N: " + nBuf.capacity() / 1024 + "Kb");
				} else {
					nBuf = null;
				}
				if(hasTexture) {
					// uv data
					uvBuf = ByteBuffer.allocateDirect(bufLen * 4 * 2);
					readBufferLE(in, uvBuf);
					System.out.println("UV: " + uvBuf.capacity() / 1024 + "Kb");
					// pixel data
					tw = readIntLE(in);
					th = readIntLE(in);
					bufLen = tw * th;
					texBuf = ByteBuffer.allocateDirect(bufLen * 2);
					readBufferLE(in, texBuf);
					System.out.println("P: " + texBuf.capacity() / 1024 + "Kb");
				} else {
					uvBuf = null;
					tw = 0;
					th = 0;
					texBuf = null;
				}
				return;
			}
			throw new RuntimeException("File Format is not recognized");
		}
		throw new RuntimeException("Native Order is not LE");
	}
	
	public static void main(String[] args) throws IOException {
		new Model(new FileInputStream("banana_index.bin"));
	}
}
