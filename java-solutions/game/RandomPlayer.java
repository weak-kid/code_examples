package game;

import java.util.Random;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class RandomPlayer implements Player {
    private final Random random;
    final private  int ROWS;
    final private  int COLS;

    public RandomPlayer(final Random random, int rows, int cols) {
        this.random = random;
        ROWS = rows;
        COLS = cols;
    }

    public RandomPlayer(int rows, int cols) {
        this(new Random(), rows, cols);
    }

    @Override
    public Move move(final Position position, final Cell cell) {
        while (true) {
            int r = random.nextInt(ROWS);
            int c = random.nextInt(COLS);
            final Move move = new Move(r, c, cell);
            if (position.isValid(move)) {
                return move;
            }
        }
    }
}
