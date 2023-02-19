import java.io.Reader;
import java.io.IOException;
import java.io.FileNotFoundException;

public class MyScanner {
	private Reader reader;
	private char[] block = new char[1024];
	private int blockSize = 0;
	private int pointer = 0;
	private char nextChar = ' ';
	private String nextObj = "";
	
	public MyScanner(Reader reader) {
		this.reader = reader;
	}
	
	public boolean nextBlock() throws IOException {
		this.blockSize = this.reader.read(block);
		this.pointer = 0;
		return this.blockSize != -1;
	}
	
	public boolean getNextChar(int move) throws IOException {
		if (pointer >= blockSize) {
			if (!nextBlock()) {
				return false;
			}
		}
		nextChar = block[pointer];
		pointer += move;
		return true;
	}
	
	public boolean lookAtNextChar() throws IOException {
		return getNextChar(0);
	}
	
	public boolean getNextChar() throws IOException {
		return getNextChar(1);
	}
	
	public static boolean chekEndOfLine(char symbol) {
		return (symbol == '\r' || symbol == '\n');
	}
	
	public boolean toNextLine() throws IOException {
		while (!chekEndOfLine(nextChar)) {
			if (!this.getNextChar()) {
				return false;
			}
		}
		if (nextChar == '\r') {
			if (!this.lookAtNextChar()) {
				return false;
			}
			if (nextChar == '\n') {
				this.getNextChar();
			}
		}
		return this.getNextChar();
	}
	
	public boolean nextInLine(Checker checker) throws IOException {
		return next(checker, true);
	}
	
	public boolean next(Checker checker, Boolean inLine) throws IOException {
		StringBuilder nextObj = new StringBuilder();
		while (!checker.check(nextChar)) {
			if (inLine && chekEndOfLine(nextChar)) {
				break;
			}
			if (!getNextChar()) {
				return false;
			}
		}
		if (inLine && chekEndOfLine(nextChar)) {
			return false;
		}
		int objSize = 0;
		while (checker.check(nextChar) && (!inLine || !chekEndOfLine(nextChar))) {
			nextObj.append(nextChar);
			objSize++;
			if (!getNextChar()) {
				break;
			}
		}
		if (objSize == 0) {
			return false;
		}
		this.nextObj = nextObj.toString();
		return true;
	}
		
	public boolean next(Checker checker) throws IOException {
		return next(checker, false);
	}
	
	public void close() throws IOException {
		reader.close();
	}
	
	public String getNext() {
		return nextObj;
	}
}

interface Checker {
	public boolean check(char value);
}
