import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.BufferedWriter;

public class WordStatInput {
	public static void main(String[] args) {
		try {
			BufferedReader reader = new BufferedReader(
					new InputStreamReader(
							new FileInputStream(args[0]),
							"utf8"
					)
			);
			try {
				BufferedWriter writer = new BufferedWriter(
						new OutputStreamWriter(
								new FileOutputStream(args[1]),
								"utf8"
						)
				);
				try {
					ArrayList<Word> words = new ArrayList<Word>();
					String line = reader.readLine();
					int numOfWord = 0;
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
									words.add(new Word(line.substring(startOfWord, i), numOfWord));
									numOfWord++;
									startOfWord = -1;
								}
							}
						}
						line = reader.readLine();
					}
					Collections.sort(words, Comparator.comparing(Word::getWord));
					words.add(new Word("", 0));
					int[] wordCounter = new int[words.size()];
					String[] answer = new String[words.size()];
					String lw = "";
					int firstmeet = 0;
					int count = 0;
					for (int i = 0; i < words.size(); i++) {
						String wordNow = words.get(i).getWord();
						if (!wordNow.equals(lw)) {
							if (count != 0) {
								wordCounter[firstmeet] = count;
								answer[firstmeet] = lw;
							}
							lw = wordNow;
							firstmeet = words.get(i).getFirstEntry();
							count = 1;
						} else {
							count++;
						}
					}
					for (int i = 0; i < wordCounter.length; i++) {
						if (wordCounter[i] != 0) {
							writer.write(answer[i] + " " + Integer.toString(wordCounter[i]) + "\n");
						}
					}
				} finally {
					writer.close();
				}
			} finally {
				reader.close();
			}
		} catch (FileNotFoundException e) {
			System.out.println("File not found:" + e.getMessage());
		} catch (IOException e) {
			System.out.println("Something went wrong");
		}
	}

	static class Word {
		String word;
		int firstEntry;

		public Word(String word, int firstEntry) {
			this.word = word;
			this.firstEntry = firstEntry;
		}

		public String getWord() {
			return this.word;
		}

		public int getFirstEntry() {
			return this.firstEntry;
		}
	}
}
