package jetdrone.convert;

import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.imageio.ImageIO;

import jetdrone.convert.util.FP;
import jetdrone.obj.Model;

public class Converter {
	
	private List<V> vertex = new ArrayList<V>();
	private List<UV> uv = new ArrayList<UV>();
	private List<N> normal = new ArrayList<N>();
	private List<F> face = new ArrayList<F>();
	private List<G> group = new ArrayList<G>();
	
	private Map<String, MTL> materials = new HashMap<String, MTL>();
	
	private List<Vertex> uVertex = new ArrayList<Vertex>();
	
	private int maxTw, maxTh;
	private int tw, th;
	private int[] texBuf;
	
	private static void writeShort(OutputStream out, short s) throws IOException {
		out.write(s & 0xff);
	    out.write((s >>> 8) & 0xff);
	}
	
	private static void writeInt(OutputStream out, int i) throws IOException {
		out.write(i & 0xff);
	    out.write((i >>> 8) & 0xff);
	    out.write((i >>> 16) & 0xff);
	    out.write((i >>> 24) & 0xff);
	}
	
	private static void writeColor(OutputStream out, int argb) throws IOException {
		//int a = (argb & 0xFF000000) >> 24;
        int r = (argb & 0x00FF0000) >> 16;
        int g = (argb & 0x0000FF00) >> 8;
        int b = (argb & 0x000000FF);
		// color are stored as RGB565
        r  = r >> 3;
        g  = g >> 2;
        b  = b >> 3;
		short color = (short) (b | (g << 5) | (r << (5 + 6)));
		writeShort(out, color);
	}
	
	private static void writeFloat(OutputStream out, boolean fixedPoint, float f) throws IOException {
		if(fixedPoint) {
			writeInt(out, FP.toFixed(f));
		} else {
			writeInt(out, Float.floatToIntBits(f));
		}
	}
	
	public void unitize() {

		double maxx, minx, maxy, miny, maxz, minz;

		double scale = 1.0;
		double[] center = new double[3];
		double[] bbox = new double[3];
		
		/* get the max/mins */
		maxx = minx = vertex.get(0).x;
		maxy = miny = vertex.get(0).y;
		maxz = minz = vertex.get(0).z;
		
		for (V v : vertex) {
			if (maxx < v.x)
				maxx = v.x;
			if (minx > v.x)
				minx = v.x;

			if (maxy < v.y)
				maxy = v.y;
			if (miny > v.y)
				miny = v.y;

			if (maxz < v.z)
				maxz = v.z;
			if (minz > v.z)
				minz = v.z;
		}

		/* calculate model width, height, and depth */
		bbox[0] = Math.abs(maxx) + Math.abs(minx);
		bbox[1] = Math.abs(maxy) + Math.abs(miny);
		bbox[2] = Math.abs(maxz) + Math.abs(minz);

		/* calculate center of the model */
		center[0] = (maxx + minx) / 2.0f;
		center[1] = (maxy + miny) / 2.0f;
		center[2] = (maxz + minz) / 2.0f;
		
		/* calculate unitizing scale factor */
		scale = 1.0 / Math.max(Math.max(bbox[0], bbox[1]), bbox[2]);
		
		for (V v : vertex) {
			v.x = (v.x - center[0]) * scale;
			v.y = (v.y - center[1]) * scale;
			v.z = (v.z - center[2]) * scale;
		}
	}
	
	public void index() {
		
		boolean found;
		Vertex vert;
		
		for(G grp : group) {
			for(int i = grp.gbegin; i < grp.gend; i++) {
				F f = face.get(i);
				found = false;
				vert = new Vertex(f.va, f.na, f.ta);
				for(Vertex vTmp : uVertex) {
					if(V.eq(vertex.get(vTmp.v), vertex.get(vert.v)) && N.eq(normal.get(vTmp.n), normal.get(vert.n)) && UV.eq(uv.get(vTmp.uv), uv.get(vert.uv))) {
						found = true;
						vert = vTmp;
						break;
					}
				}
				if(!found) {
					vert.assignId();
					uVertex.add(vert);
				}
				f.ia = vert.id;

				found = false;
				vert = new Vertex(f.vb, f.nb, f.tb);
				for(Vertex vTmp : uVertex) {
					if(V.eq(vertex.get(vTmp.v), vertex.get(vert.v)) && N.eq(normal.get(vTmp.n), normal.get(vert.n)) && UV.eq(uv.get(vTmp.uv), uv.get(vert.uv))) {
						found = true;
						vert = vTmp;
						break;
					}
				}
				if(!found) {
					vert.assignId();
					uVertex.add(vert);
				}
				f.ib = vert.id;
				
				found = false;
				vert = new Vertex(f.vc, f.nc, f.tc);
				for(Vertex vTmp : uVertex) {
					if(V.eq(vertex.get(vTmp.v), vertex.get(vert.v)) && N.eq(normal.get(vTmp.n), normal.get(vert.n)) && UV.eq(uv.get(vTmp.uv), uv.get(vert.uv))) {
						found = true;
						vert = vTmp;
						break;
					}
				}
				if(!found) {
					vert.assignId();
					uVertex.add(vert);
				}
				f.ic = vert.id;
			}
		}
		
		System.out.println("Unique index size: " + uVertex.size());
	}
	
	private static final int nearestPow2(int n) {
		int x = 1;
		while (x < n) {
			x <<= 1;
		}
		return x;
	}
	
	public Converter(String filename) throws IOException {
		this(filename, null, 0, 0);
	}
	
	public Converter(String filename, String texfilename) throws IOException {
		this(filename, texfilename, 0, 0);
	}
	
	public Converter(String filename, String texfilename, int maxTw, int maxTh) throws IOException {
		
		G currentGroup = new G(face.size());
		
		BufferedReader in = new BufferedReader(new InputStreamReader(new FileInputStream(filename)));
		String line;
		while((line = in.readLine()) != null) {
			// remove extroneous spaces
			line = line.trim();
			
			if(line.length() == 0 || line.startsWith("#")) {
				// ignore comment line
			}
			// vertices
			else if(line.matches("v\\s+.*")) {
				String tokens[] = line.substring(2).trim().split("\\s+");
				vertex.add(new V(tokens));
			}
			// texture coords
			else if(line.matches("vt\\s+.*")) {
				String tokens[] = line.substring(3).trim().split("\\s+");
				uv.add(new UV(tokens));
			}
			// normals
			else if(line.matches("vn\\s+.*")) {
				String tokens[] = line.substring(3).trim().split("\\s+");
				normal.add(new N(tokens));
			}
			// faces
			else if(line.matches("f\\s+.*")) {
				String[] tokens = line.substring(2).trim().split("\\s+");
				String[] a = tokens[0].split("/");
				String[] b = tokens[1].split("/");
				String[] c = tokens[2].split("/");
				face.add(new F(a, b, c));
			}
			// groups
			else if("g".equals(line) || line.matches("g\\s+.*")) {
				// save current group
				if(face.size() - currentGroup.gbegin > 0) {
					currentGroup.gend = face.size();
					group.add(currentGroup);
				}
				currentGroup = new G(face.size());
			}
			// material lib
			else if(line.matches("mtllib\\s+.*")) {
				// load mtllib
				String[] mtlFile = line.substring(7).trim().split("\\s+");
				BufferedReader in2 = new BufferedReader(new InputStreamReader(new FileInputStream(mtlFile[0])));
				String line2;
				MTL currentMtl = null;
				while((line2 = in2.readLine()) != null) {
					// remove extroneous spaces
					line2 = line2.trim();
					
					if(line2.length() == 0 || line2.startsWith("#")) {
						// ignore comment line
					}
					else if(line2.matches("newmtl\\s+.*")) {
						String[] tokens = line2.substring(7).trim().split("\\s+");
						if(currentMtl != null) {
							materials.put(currentMtl.name, currentMtl);
						}
						currentMtl = new MTL(tokens[0]);
					}
					else if(line2.matches("Ka\\s+.*")) {
						String[] tokens = line2.substring(3).trim().split("\\s+");
						currentMtl.ka = new K(tokens);
					}
					else if(line2.matches("Kd\\s+.*")) {
						String[] tokens = line2.substring(3).trim().split("\\s+");
						currentMtl.kd = new K(tokens);
					}
					else if(line2.matches("Ks\\s+.*")) {
						String[] tokens = line2.substring(3).trim().split("\\s+");
						currentMtl.ks = new K(tokens);
					}
					else {
						System.out.println("mtllib: don't know how to handle: " + line2);
					}
				}
				// save any current material
				if(currentMtl != null) {
					materials.put(currentMtl.name, currentMtl);
				}
				in2.close();
			}
			else {
				System.out.println("obj: don't know how to handle: " + line);
			}
		}

		// save any open group
		if(face.size() - currentGroup.gbegin > 0) {
			currentGroup.gend = face.size();
			group.add(currentGroup);
		}
		
		in.close();
		
		// process texture file if any?
		if(texfilename != null) {
			BufferedImage img = ImageIO.read(new FileInputStream(texfilename));

			tw = nearestPow2(img.getWidth());
			th = nearestPow2(img.getHeight());
			
			if(maxTw > 0 && maxTw > 0) {
				this.maxTw = nearestPow2(maxTw);
				this.maxTh = nearestPow2(maxTh);
				double diffw = (double) this.maxTw / (double) tw;
				double diffh = (double) this.maxTh / (double) th;
				double scale = Math.max(diffw, diffh);

				if(scale < 1.0) {
					// scale image
					tw *= scale;
					th *= scale;
					
					BufferedImage scaledImage = new BufferedImage((int) (img.getWidth() * scale), (int) (img.getHeight() * scale), BufferedImage.TYPE_INT_ARGB);
					Graphics2D graphics2D = scaledImage.createGraphics();
					AffineTransform xform = AffineTransform.getScaleInstance(scale, scale);
					graphics2D.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
					graphics2D.drawImage(img, xform, null);
					graphics2D.dispose();
					img = scaledImage;
				}
			}

			texBuf = new int[tw * th];
			img.getRGB(0, 0, img.getWidth(), img.getHeight(), texBuf, 0, tw);
		}
		
		// statistics
		System.out.println("Groups: " + group.size());
		System.out.println("Vertices: " + vertex.size());
		System.out.println("Normals: " + normal.size());
		System.out.println("UVs: " + uv.size());
		System.out.println("Faces: " + face.size());
		if(texfilename != null) {
			System.out.println("Texture w: " + tw);
			System.out.println("Texture h: " + th);
		}
	}
	
	public void saveIndexed(OutputStream out, boolean fixedPoint) throws IOException {
		
		short[] index = null;
		
		int[][] grp;
		
		int t = 0;
		int ii = 0;
		grp = new int[group.size()][2];
		for(int i=0; i<grp.length; i++) {
			grp[i][0] = group.get(i).gbegin * 3;
			grp[i][1] = group.get(i).gend * 3;
			t += (grp[i][1] - grp[i][0]);
			System.out.println(grp[i][0] + " - " + grp[i][1]);
		}
		
		index = new short[t];
		
		for(int i=0; i<grp.length; i++) {
			for(int j = grp[i][0] / 3; j < grp[i][1] / 3; j++) {
				F f = face.get(j);
				index[ii++] = (short) f.ia;
				index[ii++] = (short) f.ib;
				index[ii++] = (short) f.ic;
			}
		}

		// write header flag telling which components are present
		short header = 0x1ee7;
		short flag = 0;
		
		if(normal.size() > 0) flag += Model.HAS_NORMALS;
		if(uv.size() > 0 && texBuf != null) flag += Model.HAS_TEXTURE;
		
		writeShort(out, header);
		writeShort(out, flag);
		
		// write group info
		writeInt(out, grp.length);
		for(int i=0; i<grp.length; i++) {
			writeInt(out, grp[i][0]);
			writeInt(out, grp[i][1]);
		}
		
		// write index
		writeInt(out, index.length);
		for(int i=0; i<index.length; i++) {
			writeShort(out, index[i]);
		}
		
		// under indexed data, all arrays are the same length
		writeInt(out, uVertex.size());
		
		// write vertex data
		for(Vertex vert : uVertex) {
			V v = vertex.get(vert.v);
			
			writeFloat(out, fixedPoint, (float) v.x);
			writeFloat(out, fixedPoint, (float) v.y);
			writeFloat(out, fixedPoint, (float) v.z);
		}
		
		// write normal data
		if(normal.size() > 0) {
			for(Vertex vert : uVertex) {
				N n = normal.get(vert.n);
				
				writeFloat(out, fixedPoint, (float) n.x);
				writeFloat(out, fixedPoint, (float) n.y);
				writeFloat(out, fixedPoint, (float) n.z);
			}
		}
		
		if(uv.size() > 0 && texBuf != null) {
			// write uv data
			for(Vertex vert : uVertex) {
				UV _uv = uv.get(vert.uv);
				
				writeFloat(out, fixedPoint, (float) _uv.u);
				writeFloat(out, fixedPoint, (float) _uv.v);
			}
			// write pixel data
			writeInt(out, tw);
			writeInt(out, th);
			// length is tw * th
			for(int i=0; i<texBuf.length; i++) {
				writeColor(out, texBuf[i]);
			}
		}
	}
	
	public static void main(String[] args) throws IOException {
//		Converter conv = new Converter("banana.obj", "banana.jpg", 128, 128);
//		conv.unitize();
//		conv.index();
//		OutputStream out = new BufferedOutputStream(new FileOutputStream("banana_index.bin"));
//		boolean fixedPoint = true;
//		conv.saveIndexed(out, fixedPoint);
//		out.close();

		Converter conv = new Converter("mini_cooper.obj");
		conv.unitize();
		conv.index();
		OutputStream out = new BufferedOutputStream(new FileOutputStream("mini_cooper.bin"));
		boolean fixedPoint = false;
		conv.saveIndexed(out, fixedPoint);
		out.close();
	}
}
