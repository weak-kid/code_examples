package game;

import java.io.PrintStream;
import java.util.InputMismatchException;
import java.util.Scanner;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class HumanPlayer implements Player {
    private final PrintStream out;
    private final Scanner in;

    public HumanPlayer(final PrintStream out, final Scanner in) {
        this.out = out;
        this.in = in;
    }

    public HumanPlayer() {
        this(System.out, new Scanner(System.in));
    }

    @Override
    public Move move(final Position position, final Cell cell) {
        while (true) {
            out.println("Position");
            out.println(position);
            out.println(cell + "'s move");
            out.println("Enter row and column");
            int rowIn;
            int colIn;
            do {
                try {
                    rowIn = in.nextInt();
                    colIn = in.nextInt();
                    break;
                } catch (InputMismatchException e) {
                    System.out.println("You should write two numbers, try again");
                    in.nextLine();
                }
            } while (true);
            final Move move = new Move(rowIn, colIn, cell);
            if (position.isValid(move)) {
                return move;
            }
            final int row = move.getRow();
            final int column = move.getColumn();
            out.println("Move " + move + " is invalid");
        }
    }
}
