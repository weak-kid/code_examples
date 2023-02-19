package expression;

public class Subtract extends BinaryExpression {

    public Subtract(Expressions first, Expressions second) {
        super(first, second, 1, "-");
    }

    @Override
    public int evaluate(int x) {
        return first.evaluate(x) - second.evaluate(x);
    }

    @Override
    public double evaluate(double x) {
        return first.evaluate(x) - second.evaluate(x);
    }

    @Override
    public int evaluate(int x, int y, int z) {
        return first.evaluate(x, y, z) - second.evaluate(x, y, z);
    }
}
