package game;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class SequentialPlayer implements Player {
    private final int ROWS; // :NOTE: почему с большой буквы?
    private final int COLS;

    public SequentialPlayer(int rows, int cols) {
        this.ROWS = rows;
        this.COLS = cols;
    }

    @Override
    public Move move(final Position position, final Cell cell) {
        Board board = (Board) position;
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                board.makeMove(new Move(r, c, cell));
                final Move move = new Move(r, c, cell);
            }
        }
        return (new Move(0, 0, cell));
//        throw new IllegalStateException("No valid moves");
    }
}
