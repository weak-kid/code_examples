import java.util.Scanner;
import java.util.Arrays;

public class ReverseAvg {
	public static void main(String[] args) {
		Scanner sc = new Scanner(System.in);
		int[] bigArray = new int[1];
		int[] numsInLines = new int[1];
		long[] sumInLine = new long[1];
		long[] sumInCol = new long[1];
		int[] numsInCol = new int[1];
		int line = 0;
		int n = 0;
		while (sc.hasNextLine()) {
			Scanner lineSc = new Scanner(sc.nextLine());
			int numsInLine = 0;
			int col = 0;
			while (lineSc.hasNextInt()) {
				int num = lineSc.nextInt();
				sumInLine[line] += num;
				sumInCol[col] += num;
				numsInCol[col]++;
				col++;
				numsInLine++;
				if (col == sumInCol.length) {
					sumInCol = Arrays.copyOf(sumInCol, col + (col >> 1) + 1);
					numsInCol = Arrays.copyOf(numsInCol, col + (col >> 1) + 1);
				}
				bigArray[n] = num;
				n++;
				if (n == bigArray.length) {
					bigArray = Arrays.copyOf(bigArray, n + (n >> 1) + 1);
				}
			}
			numsInLines[line] = numsInLine;
			line++;
			if (line == numsInLines.length) {
				numsInLines = Arrays.copyOf(numsInLines, line + (line >> 1) + 1);
				sumInLine = Arrays.copyOf(sumInLine, line + (line >> 1) + 1);
			}
		}
		int iter = 0;
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < numsInLines[i]; j++) {
				System.out.print((sumInLine[i] + sumInCol[j] - bigArray[iter]) / (numsInCol[j] + numsInLines[i] - 1));
				iter++;
				System.out.print(" ");
			}
			System.out.println();
		}
	}
}