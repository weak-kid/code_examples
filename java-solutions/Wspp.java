import java.util.List;
import java.util.Map;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.io.OutputStream;
import java.io.Reader;
import java.io.Writer;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStreamWriter;
import java.io.InputStreamReader;
import java.io.BufferedWriter;

public class Wspp {
	public static void main(String[] args) {
		CheckWord checker = new CheckWord();
		try {
			MyScanner sc = new MyScanner(new InputStreamReader(new FileInputStream(args[0]), "utf8"));
			try {
				BufferedWriter writer = new BufferedWriter(
					new OutputStreamWriter(
						new FileOutputStream(args[1]),
						"utf8"
					)
				);
				Map<String, IntList> words = new LinkedHashMap<String, IntList>();
				int numOfWord = 0;
				while (sc.next(checker)) {
					String word = sc.getNext().toLowerCase();
					if (words.get(word) == null) {
						words.put(word, new IntList());
					}
					words.get(word).append(numOfWord);
					numOfWord++;
				}
				for (Map.Entry<String, IntList> entry : words.entrySet()) {
					IntList occurences = entry.getValue();
					writer.write(entry.getKey() + " " + Integer.toString(occurences.size()));
					for (int i = 0; i < occurences.size(); i++) {
						writer.write(" " + Integer.toString(occurences.get(i) + 1));
					}
					writer.write("\n");
				}
				writer.close();
			}  catch (IOException e) {
				System.out.println("Something went wrong with output file: " + e.getMessage());
			}
			sc.close();
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
}
