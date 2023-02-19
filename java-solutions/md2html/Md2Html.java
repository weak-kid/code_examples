package md2html;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Deque;
import java.util.List;
import java.util.Map;

public class Md2Html {
    private final static Map<String, String> MD_TO_HTML = Map.of(
            "*", "em",
            "**", "strong",
            "__", "strong",
            "`", "code",
            "--", "s",
            "_", "em"
    );

    private final static Map<Character, String> SPECIAL_SYMBOLS = Map.of(
            '<', "&lt;",
            '>', "&gt;",
            '&', "&amp;"
    );

    private final String in;
    private final String out;

    public Md2Html(String in, String out) {
        this.in = in;
        this.out = out;
    }

    public static void main(String[] args) throws IOException {
        Md2Html converter = new Md2Html(args[0], args[1]);
        List<String> answer = converter.parse();
        try (BufferedWriter writer = new BufferedWriter(
                new OutputStreamWriter(
                        new FileOutputStream(converter.out),
                        StandardCharsets.UTF_8
                )
        )) {
            for (String string : answer) {
                writer.write(string);
            }
        } catch (IOException e) {
            System.out.println("Something gone wrong with output: " + e.getMessage());
        }
    }

    private List<String> parse() {
        StringBuilder inFile = new StringBuilder();
        try (BufferedReader reader = new BufferedReader(
                new InputStreamReader(
                        new FileInputStream(in),
                        StandardCharsets.UTF_8
                )
        )) {
            char[] block = new char[4096];
            int blockSize = reader.read(block);
            while (blockSize != -1) {
                inFile.append(Arrays.copyOf(block, blockSize));
                blockSize = reader.read(block);
            }
            inFile.append('\n');
        } catch (IOException e) {
            System.out.println("Something gone wrong with input: " + e.getMessage());
        }
        String input = inFile.toString();
        int nextParagraph = 2;
        int headerLevel = 0;
        int headerLevelCloser = 0;
        String[] checkOrder = {"**", "*", "__", "`", "--", "_"};
        Map<String, int[]> quantityOfMarks = Map.of(
                "*", new int[1],
                "**", new int[1],
                "__", new int[1],
                "`", new int[1],
                "--", new int[1],
                "_", new int[1]
        );
        int position = 0;
        Deque<PairStringInt> marks = new ArrayDeque<>();
        List<String> answer = new ArrayList<>();
        for (int i = 0; i < input.length(); i++) {
            if (input.charAt(i) == '\n' || input.charAt(i) == '\r'
                    || input.startsWith("\r\n", i)) {
                nextParagraph++;
                if (nextParagraph == 2) {
                    for (Map.Entry<String, int[]> entry : quantityOfMarks.entrySet()) {
                        entry.getValue()[0] = 0;
                    }
                    answer.remove(answer.size() - 1);
                    if (headerLevelCloser != 0) {
                        answer.add(closerToHtml("h" + headerLevelCloser));
                        headerLevelCloser = 0;
                    } else {
                        answer.add(closerToHtml("p"));
                    }
                    answer.add(System.lineSeparator());
                }
                if (nextParagraph == 1) {
                    answer.add(System.lineSeparator());
                }
                if (input.startsWith("\r\n", i)) {
                    i++;
                }
            } else {
                int symb = 0;
                if (input.charAt(i) == '#' && (nextParagraph >= 2 || headerLevel != 0)) {
                    symb++;
                    headerLevel++;
                } else if (headerLevel != 0 && input.charAt(i) == ' ') {
                    answer.add(openerToHtml("h" + headerLevel));
                    headerLevelCloser = headerLevel;
                    headerLevel = 0;
                    symb++;
                } else {
                    if (nextParagraph >= 2 || headerLevel != 0) {
                        answer.add(openerToHtml("p"));
                        for (int j = 0; j < headerLevel; j++) {
                            answer.add("#");
                        }
                    }
                    headerLevel = 0;
                }
                nextParagraph = 0;
                if (input.charAt(i) == '[') {
                    symb++;
                    position = answer.size();
                    answer.add(null);
                }
                if (input.charAt(i) == ']') {
                    symb++;
                    i += 2;
                    StringBuilder href = new StringBuilder();
                    while (input.charAt(i) != ')') {
                        href.append(input.charAt(i));
                        i++;
                    }
                    answer.set(position, openerToHtml("a href='" + href + "'"));
                    answer.add(closerToHtml("a"));
                }
                for (Map.Entry<Character, String> entry : SPECIAL_SYMBOLS.entrySet()) {
                    if (input.charAt(i) == entry.getKey()) {
                        answer.add(entry.getValue());
                        symb++;
                    }
                }
                for (String mdMark : checkOrder) {
                    if (i > 0 && input.charAt(i - 1) == '\\') {
                        answer.remove(answer.size() - 1);
                        break;
                    }
                    String htmlMark = MD_TO_HTML.get(mdMark);
                    if (i + mdMark.length() <= input.length()) {
                        if (input.startsWith(mdMark, i)) {
                            symb++;
                            int occur = quantityOfMarks.get(mdMark)[0];
                            if (occur > 0) {
                                while (!marks.getLast().string.equals(mdMark)) {
                                    quantityOfMarks.get(marks.getLast().string)[0]--;
                                    marks.removeLast();
                                }
                                answer.set(marks.getLast().num, openerToHtml(htmlMark));
                                quantityOfMarks.get(mdMark)[0]--;
                                marks.removeLast();
                                answer.add(closerToHtml(htmlMark));
                            } else {
                                quantityOfMarks.get(mdMark)[0]++;
                                marks.add(new PairStringInt(mdMark, answer.size()));
                                answer.add(mdMark);
                            }
                            i += mdMark.length() - 1;
                            break;
                        }
                    }
                }
                if (symb == 0) {
                    answer.add(String.valueOf(input.charAt(i)));
                }
            }
        }
        return answer;
    }

    static private String openerToHtml(String opener) {
        return "<" + opener + ">";
    }

    static private String closerToHtml(String closer) {
        return openerToHtml("/" + closer);
    }

    private static class PairStringInt {
        private final String string;
        private final int num;

        public PairStringInt(String string, int num) {
            this.string = string;
            this.num = num;
        }
    }
}
