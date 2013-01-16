package jetdrone.convert;

class V {
	double x;
	double y;
	double z;
	
	V(String[] args) {
		if(args.length > 0) x = Double.valueOf(args[0]);
		if(args.length > 1) y = Double.valueOf(args[1]);
		if(args.length > 2) z = Double.valueOf(args[2]);
	}
	
	static boolean eq(V v1, V v2) {
		if(v1 == null && v2 == null) return true;
		if(v1 != null && v2 != null) {
			return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
		}
		return false;
	}
}
