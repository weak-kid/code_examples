public class SumLongOctal {
	public static void main(String[] args) {
		long ans = 0;
		for (int i = 0; i < args.length; i++) {
			String strNow = args[i] + ' ';
			int numStart = -1;
			for (int j = 0; j < strNow.length(); j++) {
				if (!Character.isWhitespace(strNow.charAt(j))) {
					if (numStart == -1) {
						numStart = j;
					}
				} else if (numStart != -1) {
					int type = 10;
					int numEnd = j;
					if (strNow.charAt(j - 1) == 'o' || strNow.charAt(j - 1) == 'O') {
						type = 8;
						numEnd -= 1;
					}
					ans += Long.parseLong(strNow.substring(numStart, numEnd), type);
					numStart = -1;
				}
			}
		}
		System.out.println(ans);
	}
}