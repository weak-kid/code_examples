package expression;

import java.util.Objects;

public class Const extends Expressions {
    final private int intConst;
    final private double doubleConst;
    private boolean flagIsDouble = false;

    public Const(int c) {
        this.intConst = c;
        this.doubleConst = c;
    }

    public Const(double c) {
        this.doubleConst = c;
        this.intConst = 0;
        flagIsDouble = true;
    }

    @Override
    public int evaluate(int x) {
        return intConst;
    }

    @Override
    public double evaluate(double x) {
        return doubleConst;
    }

    @Override
    public int evaluate(int x, int y, int z) {
        return intConst;
    }

    @Override
    public String toString() {
        if (flagIsDouble) {
            return Double.toString(doubleConst);
        }
        return Integer.toString(intConst);
    }

    @Override
    public String toMiniString() {
        return this.toString();
    }

    @Override
    public boolean equals(Object obj) {
        return obj != null && obj.getClass() == Const.class &&
                (intConst == ((Const) obj).intConst && !flagIsDouble ||
                doubleConst == ((Const) obj).doubleConst && flagIsDouble);
    }

    @Override
    public int hashCode() {
        if (flagIsDouble) {
            return Objects.hash(doubleConst);
        }
        return Objects.hash(intConst);
    }
}
