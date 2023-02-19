package game;

import java.util.Arrays;
import java.util.Map;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class TicTacToeBoard implements Board {
    private static final Map<Cell, Character> SYMBOLS = Map.of(
            Cell.X, 'X',
            Cell.O, 'O',
            Cell.E, '.',
            Cell.L, '|',
            Cell.M, '-',
            Cell.F, '#'
    );

    private final Cell[][] cells;
    private Cell turn;
    public final int ROWS;
    public final int COLS;
    private final int REQ;
    private int empty;
    private final int quantityOfPlayers;

    public TicTacToeBoard(int n, int m, int k, int quantityOfPlayers) {
        ROWS = n;
        COLS = m;
        REQ = k;
        empty = n * m;
        this.cells = new Cell[n][m];
        for (Cell[] row : cells) {
            Arrays.fill(row, Cell.E);
        }
        turn = Cell.X;
        this.quantityOfPlayers = quantityOfPlayers;
    }

    public TicTacToeBoard(int n, int m, int k, int[] forbiddenRows, int[] forbiddenCols, int quantityOfPlayers) {
        ROWS = n;
        COLS = m;
        REQ = k;
        empty = n * m;
        this.cells = new Cell[n][m];
        for (Cell[] row : cells) {
            Arrays.fill(row, Cell.E);
        }
        for (int i = 0; i < forbiddenRows.length; i++) {
            empty--;
            cells[forbiddenRows[i]][forbiddenCols[i]] = Cell.F;
        }
        turn = Cell.X;
        this.quantityOfPlayers = quantityOfPlayers;
    }

    public TicTacToeBoard(int n, int m, int k, int[] forbiddenRows, int[] forbiddenCols) {
        this(n, m, k, forbiddenRows, forbiddenCols, 2);
    }

    public TicTacToeBoard(int n, int m, int k) {
        this(n, m, k, 2);
    }

    @Override
    public Position getPosition() {
        return new TicTacToePosition(this);
    }

    @Override
    public Cell getCell() {
        return turn;
    }

    @Override
    public Result makeMove(final Move move) {
        if (!isValid(move)) {
            return Result.LOSE;
        }

        cells[move.getRow()][move.getColumn()] = move.getValue();

        int row = move.getRow();
        int col = move.getColumn();
        int onRow = 1 + counter(row, col, 0, 1) + counter(row, col, 0, -1);
        int onCol = 1 + counter(row, col, 1, 0) + counter(row, col, -1, 0);
        int mainDiag = 1 + counter(row, col, 1, 1) + counter(row, col, -1, -1);
        int secDiag = 1 + counter(row, col, -1, 1) + counter(row, col, 1, -1);
        if (onRow >= REQ || onCol >= REQ || mainDiag >= REQ || secDiag >= REQ) {
            return Result.WIN;
        }
        empty--;
        if (empty == 0) {
            return Result.DRAW;
        }
        if (turn == Cell.X) {
            turn = Cell.O;
        } else if (turn == Cell.O) {
            turn = Cell.M;
            if (quantityOfPlayers == 2) {
                turn = Cell.X;
            }
        } else if (turn == Cell.M) {
            turn = Cell.L;
            if (quantityOfPlayers == 3) {
                turn = Cell.X;
            }
        } else if (turn == Cell.L) {
            turn = Cell.X;
        }
        return Result.UNKNOWN;
    }


    public boolean isValid(final Move move) {
        return 0 <= move.getRow() && move.getRow() < ROWS
                && 0 <= move.getColumn() && move.getColumn() < COLS
                && cells[move.getRow()][move.getColumn()] == Cell.E
                && turn == getCell();
    }


    public Cell getCell(final int r, final int c) {
        return cells[r][c];
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("   ");
        for (int i = 0; i < COLS; i++) {
            sb.append(i);
        }
        for (int r = 0; r < ROWS; r++) {
            sb.append("\n");
            sb.append(String.format("%3d", r));
            for (int c = 0; c < COLS; c++) {
                sb.append(SYMBOLS.get(cells[r][c]));
            }
        }
        return sb.toString();
    }

    private int counter(int row, int col, int rowMul, int colMul) {
        int counter = 0;
        for (int i = 1; i < REQ; i++) {
            if (0 > row + (rowMul * i) || row + (rowMul * i) >= ROWS) {
                break;
            }
            if (0 > col + (colMul * i) || col + (colMul * i) >= COLS) {
                break;
            }
            if (cells[row + (rowMul * i)][col + (colMul * i)] == turn) {
                counter += 1;
            } else {
                break;
            }
        }
        return counter;
    }
}
