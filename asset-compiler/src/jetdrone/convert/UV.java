package jetdrone.convert;

class UV {
	double u;
	double v;
	
	UV(String[] args) {
		if(args.length > 0) u = Double.valueOf(args[0]);
		if(args.length > 1) v = 1.f - Double.valueOf(args[1]);
	}
	
	static boolean eq(UV uv1, UV uv2) {
		if(uv1 == null && uv2 == null) return true;
		if(uv1 != null && uv2 != null) {
			return uv1.u == uv2.u && uv1.v == uv2.v;
		}
		return false;
	}
}
