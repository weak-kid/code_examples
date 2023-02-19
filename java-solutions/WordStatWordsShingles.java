import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.InputStreamReader;
import java.io.BufferedWriter;
import java.io.BufferedReader;

public class WordStatWordsShingles {
	public static final int SUBSTR_SIZE = 3;
	public static void main(String[] args) {
		List<String> words = new ArrayList<String>();
		try {
			BufferedReader reader = new BufferedReader(
					new InputStreamReader(
							new FileInputStream(args[0]),
							"utf8"
					)
			);
			String line = reader.readLine();
			while (line != null) {
				line = line.toLowerCase() + " ";
				int startOfWord = -1;
				for (int i = 0; i < line.length(); i++) {
					if (Character.isLetter(line.charAt(i)) || line.charAt(i) == '\'' || Character.getType(line.charAt(i)) == Character.DASH_PUNCTUATION) {
						if (startOfWord == -1) {
							startOfWord = i;
						}
					} else {
						if (startOfWord != -1) {
							if (i - startOfWord <= SUBSTR_SIZE) {
								words.add(line.substring(startOfWord, i));
							} else {
								for (int j = startOfWord; j <= i - SUBSTR_SIZE; j++) {
									words.add(line.substring(j, j + SUBSTR_SIZE));
								}
							}
							startOfWord = -1;
						}
					}
				}
				line = reader.readLine();
			}
			reader.close();
		} catch (FileNotFoundException e) {
			System.out.println("Input file not found: " + e.getMessage());
		} catch (IOException e) {
			System.out.println("Something wrong with input file: " + e.getMessage());
		}
		try {
			BufferedWriter writer = new BufferedWriter(
					new OutputStreamWriter(
							new FileOutputStream(args[1]),
							"utf8"
					)
			);
			Collections.sort(words);
			words.add("");
			String prevWord = "";
			int count = 0;
			for (int i = 0; i < words.size(); i++) {
				String wordNow = words.get(i);
				if (!wordNow.equals(prevWord)) {
					if (count != 0) {
						writer.write(prevWord + " " + Integer.toString(count) + "\n");
					}
					prevWord = wordNow;
					count = 1;
				} else {
					count++;
				}
			}
			writer.close();
		}  catch (IOException e) {
			System.out.println("Something wrong with input file: " + e.getMessage());
		}
	}
}