package game;

import java.util.InputMismatchException;
import java.util.Scanner;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
public class Main {
    public static void main(String[] args) {
        Scanner in = new Scanner(System.in);
        System.out.println("Write board size(rows, cols), and size of winning sequence");
        int n;
        int m;
        int k;
        do {
            try {
                n = in.nextInt();
                m = in.nextInt();
                k = in.nextInt();
                break;
            } catch (InputMismatchException e) {
                System.out.println("You should write two numbers, try again");
                in.nextLine();
            }
        } while (true);
        System.out.println("""
                If you want to play tournament write Y
                Board with forbidden diagonal(desk 11x11): F
                Otherwise write anything else""");
        String type = in.next();
        if (type.equals("F")){
            final int fieldSize = 11;
            final Game game = new Game(false,
                    new HumanPlayer(),
                    new RandomPlayer(fieldSize, fieldSize));
            int[] forbiddenRows = new int[fieldSize];
            int[] forbiddenCols = new int[fieldSize];
            for (int i = 0; i < fieldSize; i++) {
                forbiddenRows[i] = i;
                forbiddenCols[i] = i;
            }
            int result = game.play(new TicTacToeBoard(fieldSize, fieldSize, 5, forbiddenRows, forbiddenCols));
        } else if (type.equals("Y")) {
            Player[] players = new Player[4];
            for (int i = 1; i < 4; i++) {
                players[i] = new RandomPlayer(n, m);
            }
            players[0] = new HumanPlayer();
            Tournament tournament = new Tournament(false, n, m, k, 4, players);
            tournament.play();
        } else {
            boolean nextGame = false;
            do {
                final Game game = new Game(true, new HumanPlayer(), new RandomPlayer(n, m), new RandomPlayer(n, m), new RandomPlayer(n, m));
                int result;
                result = game.play(new TicTacToeBoard(n, m, k, 4));
                System.out.println("Game result: " + result);
                System.out.println("""
                        If you want to play next game write Y
                        If you want to change field size write C
                        Otherwise write something else""");
                String yOrNo = in.next();
                nextGame = yOrNo.equals("Y");
                if (yOrNo.equals("C")) {
                    nextGame = true;
                    do {
                        try {
                            n = in.nextInt();
                            m = in.nextInt();
                            k = in.nextInt();
                            break;
                        } catch (InputMismatchException e) {
                            System.out.println("You should write two numbers, try again");
                            in.nextLine();
                        }
                    } while (true);
                }
            } while (nextGame);
        }
    }
}
