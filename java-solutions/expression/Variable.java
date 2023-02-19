package expression;

import javax.naming.NamingException;
import java.util.Objects;

public class Variable extends Expressions {
    String x;

    public Variable(String x) {
        this.x = x;
    }

    @Override
    public int evaluate(int x) {
        return x;
    }

    @Override
    public double evaluate(double x) {
        return x;
    }

    @Override
    public int evaluate(int x, int y, int z) {
        if (this.x.equals("x")) {
            return x;
        }
        if (this.x.equals("y")) {
            return y;
        }
        if (this.x.equals("z")) {
            return z;
        }
        return x;
    }

    @Override
    public String toString() {
        return x;
    }

    @Override
    public String toMiniString() {
        return x;
    }

    @Override
    public boolean equals(Object obj) {
        return obj != null && obj.getClass() == Variable.class && x.equals(((Variable) obj).x);
    }

    @Override
    public int hashCode() {
        return Objects.hash(x);
    }
}
