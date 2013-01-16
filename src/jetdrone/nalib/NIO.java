package jetdrone.nalib;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class NIO {

	public static final short readShortLE(final InputStream in) throws IOException {
		final byte[] buffer = new byte[2];
		
		if(in.read(buffer) != 2) {
			throw new IOException();
		}
		
		return (short) ((buffer[0] & 0xFF) | (buffer[1] & 0xFF) << 8);
	}
	
	public static final int readIntLE(final InputStream in) throws IOException {
		final byte[] buffer = new byte[4];
		
		if(in.read(buffer) != 4) {
			throw new IOException();
		}
		
		return (buffer[0] & 0xFF) | (buffer[1] & 0xFF) << 8 | (buffer[2] & 0xFF) << 16 | (buffer[3] & 0xFF) << 24;
	}
	
	public static final void readBufferLE(final InputStream in, final ByteBuffer buf) throws IOException {
		final byte[] buffer = new byte[4096];
		int bufLen = buf.capacity();
		int readBytes = 0;
		
		buf.order(ByteOrder.LITTLE_ENDIAN);
		
		while(bufLen > 0) {
			if(bufLen > buffer.length) {
				readBytes = in.read(buffer, 0, buffer.length);
				buf.put(buffer, 0, buffer.length);
				bufLen -= readBytes;
			} else {
				readBytes = in.read(buffer, 0, bufLen);
				buf.put(buffer, 0, bufLen);
				bufLen -= readBytes;
			}
		}
	}
	
	
}
