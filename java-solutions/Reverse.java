import java.util.Arrays;
import java.io.Reader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Reverse {
	public static void main(String[] args) {
		try {
			MyScanner sc = new MyScanner(new InputStreamReader(System.in));
			int[] bigArray = new int[1];
			int[] numsInLines = new int[1];
			int line = 0;
			int n = 0;
			boolean c = true;
			while (c) {
				int numsInLine = 0;
				while (sc.nextInLine(new CheckInt())) {
					bigArray[n] = Integer.parseInt(sc.getNext());
					numsInLine++;
					n++;
					if (n == bigArray.length) {
						bigArray = Arrays.copyOf(bigArray, n * 2);
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
					System.out.print(bigArray[j - iter]);
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
			return (Character.isDigit(let) || let == '-');
		}
	}
}
