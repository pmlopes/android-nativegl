package jetdrone.convert;

class F {
	int va, vb, vc;
	int ta, tb, tc;
	int na, nb, nc;
	// index
	int ia, ib, ic;
	
	F(String[] a, String[] b, String[] c) {
		if(a.length > 0) va = "".equals(a[0]) ? -1 : Integer.valueOf(a[0]) - 1;
		if(a.length > 1) ta = "".equals(a[1]) ? -1 : Integer.valueOf(a[1]) - 1;
		if(a.length > 2) na = "".equals(a[2]) ? -1 : Integer.valueOf(a[2]) - 1;

		if(b.length > 0) vb = "".equals(b[0]) ? -1 : Integer.valueOf(b[0]) - 1;
		if(b.length > 1) tb = "".equals(b[1]) ? -1 : Integer.valueOf(b[1]) - 1;
		if(b.length > 2) nb = "".equals(b[2]) ? -1 : Integer.valueOf(b[2]) - 1;

		if(c.length > 0) vc = "".equals(c[0]) ? -1 : Integer.valueOf(c[0]) - 1;
		if(c.length > 1) tc = "".equals(c[1]) ? -1 : Integer.valueOf(c[1]) - 1;
		if(c.length > 2) nc = "".equals(c[2]) ? -1 : Integer.valueOf(c[2]) - 1;
	}
}
