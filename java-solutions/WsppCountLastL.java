import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Map;
import java.util.LinkedHashMap;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.InputStreamReader;
import java.io.BufferedWriter;

public class WsppCountLastL {
	public static void main(String[] args) {
		CheckWord checker = new CheckWord();
		try {
			MyScanner sc = new MyScanner(new InputStreamReader(new FileInputStream(args[0]), StandardCharsets.UTF_8));
			try {
				BufferedWriter writer = new BufferedWriter(
						new OutputStreamWriter(
								new FileOutputStream(args[1]),
								StandardCharsets.UTF_8
						)
				);
				Map<String, IntList> wordsPosInLine = new LinkedHashMap<>();
				boolean notLastLine = true;
				int numOfLine = 0;
				while (notLastLine) {
					int numOfWord = 0;
					while (sc.nextInLine(checker)) {
						String word = sc.getNext().toLowerCase();
						IntList occurences = wordsPosInLine.get(word);
						if (wordsPosInLine.get(word) == null) {
							occurences = new IntList();
							occurences.append(0);
							occurences.append(-1);
							wordsPosInLine.put(word, occurences);
						}
						occurences.change(0, occurences.get(0) + 1);
						if (occurences.get(1) == -1) {
							occurences.append(numOfWord);
							occurences.change(1, numOfLine);
						} else {
							if (occurences.get(1) != numOfLine) {
								occurences.append(numOfWord);
								occurences.change(1, numOfLine);
							} else {
								occurences.pop();
								occurences.append(numOfWord);
							}
						}
						numOfWord++;
					}
					numOfLine++;
					notLastLine = sc.toNextLine();
				}
				List<Word> allwords = new ArrayList<>();
				for (Map.Entry<String, IntList> entry : wordsPosInLine.entrySet()) {
					allwords.add(new Word(entry.getKey(), entry.getValue()));
				}
				Collections.sort(allwords, Comparator.comparing(Word::getCount));
				for (int i = 0; i < allwords.size(); i++) {
					writer.write(allwords.get(i).word + " " + allwords.get(i).lastOccurencesL.get(0));
					for (int j = 2; j < allwords.get(i).lastOccurencesL.size(); j++) {
						writer.write(" " + (allwords.get(i).lastOccurencesL.get(j) + 1));
					}
					writer.write(System.lineSeparator());
				}
				writer.close();
			}  catch (IOException e) {
				System.out.println("Something went wrong with output file: " + e.getMessage());
			} finally {
				sc.close();
			}
		} catch (FileNotFoundException e) {
			System.out.println("Input file not found:" + e.getMessage());
		} catch (IOException e) {
			System.out.println("Something went wrong with input file: " + e.getMessage());
		}
	}

	public static class CheckWord implements Checker {
		public boolean check(char let) {
			return (Character.isLetter(let) || let == '\'' || Character.getType(let) == Character.DASH_PUNCTUATION);
		}
	}


	static class Word {
		public String word;
		public IntList lastOccurencesL;

		public Word(String word, IntList lastOccurencesL) {
			this.word = word;
			this.lastOccurencesL = lastOccurencesL;
		}

		public int getCount() {
			return  lastOccurencesL.get(0);
		}
	}

}
