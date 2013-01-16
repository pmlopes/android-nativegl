package jetdrone.convert;

class N {
	double x;
	double y;
	double z;
	
	N(String[] args) {
		if(args.length > 0) x = Double.valueOf(args[0]);
		if(args.length > 1) y = Double.valueOf(args[1]);
		if(args.length > 2) z = Double.valueOf(args[2]);
		
		double d = Math.sqrt(x*x + y*y + z*z);
		
		if(d == 0.d) {
			x = 1;
			y = 0;
			z = 0;
		} else {
			x = x / d;
			y = y / d;
			z = z / d;
		}
	}

	static boolean eq(N n1, N n2) {
		if(n1 == null && n2 == null) return true;
		if(n1 != null && n2 != null) {
			return n1.x == n2.x && n1.y == n2.y && n1.z == n2.z;
		}
		return false;
	}
}
