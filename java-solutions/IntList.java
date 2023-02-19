import java.util.Arrays;

public class IntList {
	private int[] intList;
	private int listSize;
	
	public IntList(int[] intList) {
		this.intList = intList;
		listSize = intList.length;
	}
	
	public IntList() {
		this.intList = new int[1];
		listSize = 0;
	}
	
	public void append(int x) {
		if (listSize == intList.length) {
			resize();
		}
		intList[listSize] = x;
		listSize++;
	}
	
	public void resize() {
		intList = Arrays.copyOf(intList, listSize + (listSize >> 1) + 1);
	}
	
	public int get(int i) {
		return intList[i];
	}
	
	public int size() {
		return listSize;
	}
	
	public void pop() {
		listSize--;
	}
	
	public void change(int i, int x) {
		intList[i] = x;
	}
}