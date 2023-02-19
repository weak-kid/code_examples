package game;

public class TicTacToePosition implements Position {
    final private TicTacToeBoard tictactoeboard;

    public TicTacToePosition(TicTacToeBoard tictactoe) {
        this.tictactoeboard = tictactoe;
    }

    @Override
    public Cell getCell(final int r, final int c) {
        return tictactoeboard.getCell(r, c);
    }

    @Override
    public boolean isValid(final Move move) {
        return 0 <= move.getRow() && move.getRow() < tictactoeboard.ROWS
                && 0 <= move.getColumn() && move.getColumn() < tictactoeboard.COLS
                && tictactoeboard.getCell(move.getRow(), move.getColumn()) == Cell.E;
    }

    @Override
    public String toString() {
        return tictactoeboard.toString();
    }
}
