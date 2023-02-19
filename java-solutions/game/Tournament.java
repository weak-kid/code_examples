package game;

public class Tournament {
    private final int n;
    private final int m;
    private final int quantity;
    private final Player[] players;
    private final int k;
    private final boolean log;

    public Tournament(boolean log, int n, int m, int k, int quantity, Player[] players) {
        this.n = n;
        this.m = m;
        this.k = k;
        this.quantity = quantity;
        this.players = players;
        this.log = log;
    }

    public void play() {
        int[][] table = new int[quantity][quantity];
        for (int i = 0; i < quantity; i++) {
            for (int j = 0; j < quantity; j++) {
                if (i != j) {
                    Game game = new Game(log, players[i], players[j]);
                    int result = game.play(new TicTacToeBoard(n, m, k));
                    if (result == 1) {
                        table[i][j] += 3;
                    }
                    if (result == 0) {
                        table[i][j] += 1;
                        table[j][i] += 1;
                    }
                    if (result == 2) {
                        table[j][i] += 3;
                    }
                }
            }
        }
        System.out.print("TOURNAMENT RESULTS:\n  ");
        for (int i = 0; i < quantity; i++) {
            System.out.print(i);
        }
        System.out.println();
        for (int i = 0; i < quantity; i++) {
            System.out.print(i);
            System.out.print(" ");
            int result = 0;
            for (int j = 0; j < quantity; j++) {
                if (i == j) {
                    System.out.print("#");
                    continue;
                }
                System.out.print(table[i][j]);
                result += table[i][j];
            }
            System.out.println(" SUM:  " + result);
        }
    }
}
