package jetdrone.convert;

public class K {
	
	double r;
	double g;
	double b;
	
	K(String[] args) {
		if(args.length > 0) r = Double.valueOf(args[0]);
		if(args.length > 1) g = Double.valueOf(args[1]);
		if(args.length > 2) b = Double.valueOf(args[2]);
	}

}
