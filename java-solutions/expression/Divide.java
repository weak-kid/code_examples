package expression;

public class Divide extends BinaryExpression {

    public Divide(Expressions first, Expressions second) {
        super(first, second, 2, "/");
    }

    @Override
    public int evaluate(int x) {
        return first.evaluate(x) / second.evaluate(x);
    }

    @Override
    public double evaluate(double x) {
        return first.evaluate(x) / second.evaluate(x);
    }

    @Override
    public int evaluate(int x, int y, int z) {
        return first.evaluate(x, y, z) / second.evaluate(x, y, z);
    }
}
