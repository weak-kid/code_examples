import java.util.Arrays;
import java.io.IOException;
import java.io.InputStreamReader;

public class ReverseOctAbc {
	public static void main(String[] args) {
		CheckInt checker = new CheckInt();
		try {
			MyScanner sc = new MyScanner(new InputStreamReader(System.in));
			int[] bigNumArray = new int[1];
			int[] numsInLines = new int[1];
			int line = 0;
			int n = 0;
			boolean c = true;
			while (c) {
				int numsInLine = 0;
				while (sc.nextInLine(checker)) {
					String nextIntInString = sc.getNext().toLowerCase();
					int nextIntLen = nextIntInString.length();
					int radix = 10;
					boolean isNegative = false;
					if (nextIntInString.charAt(0) == '-') {
						isNegative = true;
						nextIntInString = nextIntInString.substring(1, nextIntLen);
						nextIntLen--;
					}
					if (nextIntInString.charAt(nextIntLen - 1) == 'o') {
						radix = 8;
						nextIntLen--;
						nextIntInString = nextIntInString.substring(0, nextIntLen);
					}
					char[] numInCharArray = nextIntInString.toCharArray();
					for (int i = 0; i < nextIntLen; i++) {
						if ('a' <= numInCharArray[i] && numInCharArray[i] <= 'j') {
							numInCharArray[i] = (char)('0' + numInCharArray[i] - 'a');
						}
					}
					bigNumArray[n] = Integer.parseUnsignedInt(new String(numInCharArray), radix);
					if (isNegative) {
						bigNumArray[n] = -bigNumArray[n];
					}
					numsInLine++;
					n++;
					if (n == bigNumArray.length) {
						bigNumArray = Arrays.copyOf(bigNumArray, n * 2);
					}
				}
				numsInLines[line] = numsInLine;
				line++;
				if (line == numsInLines.length) {
					numsInLines = Arrays.copyOf(numsInLines, line * 2);
				}
				c = sc.toNextLine();
			}
			int i = line - 1;
			int j = n - 1;
			while (i >= 0) {
				int iter = 0;
				while (iter < numsInLines[i]) {
					System.out.print(bigNumArray[j - iter]);
					System.out.print(' ');
					iter++;
				}
				System.out.println();
				j -= numsInLines[i];
				i--;
			}
			sc.close();
		} catch (IOException e) {
			System.out.println("Something went wrong with input: " + e.getMessage());
		}
	}

	public static class CheckInt implements Checker {
		public boolean check(char let) {
			return (!Character.isWhitespace(let));
		}
	}
}
