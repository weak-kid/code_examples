package game;

import java.security.spec.ECField;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class Game {
    private final boolean log;
    private final Player player1, player2, player3, player4;
    private final int quantityOfPlayers;

    public Game(final boolean log, final Player player1, final Player player2) {
        this.log = log;
        this.player1 = player1;
        this.player2 = player2;
        this.player3 = null;
        this.player4 = null;
        quantityOfPlayers = 2;
    }

    public Game(final boolean log, final Player player1, final Player player2, final Player player3) {
        this.log = log;
        this.player1 = player1;
        this.player2 = player2;
        this.player3 = player3;
        this.player4 = null;
        quantityOfPlayers = 3;
    }

    public Game(final boolean log, final Player player1, final Player player2, final Player player3, final Player player4) {
        this.log = log;
        this.player1 = player1;
        this.player2 = player2;
        this.player3 = player3;
        this.player4 = player4;
        quantityOfPlayers = 4;
    }

    public int play(Board board) {
        while (true) {
            final int result1 = move(board, player1, 1);
            if (result1 != -1) {
                return result1;
            }
            final int result2 = move(board, player2, 2);
            if (result2 != -1) {
                return result2;
            }
            if (quantityOfPlayers >= 3) {
                final int result3 = move(board, player3, 3);
                if (result3 != -1) {
                    return result3;
                }
            }
            if (quantityOfPlayers == 4) {
                final int result4 = move(board, player4, 4);
                if (result4 != -1) {
                    return result4;
                }
            }
        }
    }

    private int move(final Board board, final Player player, final int no) {
        final Move move;
        try {
            move = player.move(board.getPosition(), board.getCell());
        } catch (Exception e) {
            System.out.println("Looks like player " + no + " tried to cheat");
            log("undefined behavior");
            return 1000; // :NOTE: что за тысяча...
        }
        final Result result = board.makeMove(move);
        log("Player " + no + " move: " + move);
        log("Position:\n" + board);
        if (result == Result.WIN) {
            log("Player " + no + " won");
            return no;
        } else if (result == Result.DRAW) {
            log("Draw");
            return 0;
        } else {
            return -1;
        }
    }

    private void log(final String message) {
        if (log) {
            System.out.println(message);
        }
    }
}
