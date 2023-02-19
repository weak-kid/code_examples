public class Sum {
	public static void main(String[] args) {
		int ans = 0;
		for (int i = 0; i < args.length; i++) {
			String strNow = args[i] + ' ';
			int numStart = -1;
			for (int j = 0; j < strNow.length(); j++) {
				if (!Character.isWhitespace(strNow.charAt(j))) {
					if (numStart == -1) {
						numStart = j;
					}
				} else if (numStart != -1) {
					ans += Integer.parseInt(strNow.substring(numStart, j));
					numStart = -1;
				}
			}
		}
		System.out.println(ans);
	}
}