package expression;

import java.util.Objects;

public abstract class BinaryExpression extends Expressions implements Expression, DoubleExpression, TripleExpression {
    protected final Expressions first;
    protected final Expressions second;
    protected final int priority;
    protected final String exp;

    protected BinaryExpression(Expressions first, Expressions second, int priority, String exp) {
        this.first = first;
        this.second = second;
        this.priority = priority;
        this.exp = exp;
    }


    @Override
    public boolean equals(Object obj) {
        return obj instanceof BinaryExpression &&
                exp.equals(((BinaryExpression) obj).exp) &&
                first.equals(((BinaryExpression) obj).first) &&
                second.equals(((BinaryExpression) obj).second);
    }

    @Override
    public String toMiniString() {
        StringBuilder sb = new StringBuilder();
        if (first instanceof BinaryExpression && ((BinaryExpression) first).priority < priority) {
            sb.append("(");
            sb.append(first.toMiniString());
            sb.append(")");
        } else {
            sb.append(first.toMiniString());
        }
        sb.append(" ").append(exp).append(" ");
        if (second instanceof BinaryExpression &&
                (((BinaryExpression) second).priority < priority ||
                exp.equals("-") && ((BinaryExpression) second).priority == 1 ||
                exp.equals("/") && ((BinaryExpression) second).priority == 2 ||
                exp.equals("*") && ((BinaryExpression) second).exp.equals("/"))) {
            sb.append("(");
            sb.append(second.toMiniString());
            sb.append(")");
        } else {
            sb.append(second.toMiniString());
        }
        return sb.toString();
    }


    public String toString() {
        return "(" + first.toString() + " " + exp + " " + second.toString() + ")";
    }

    @Override
    public int hashCode() {
        return Objects.hash(first, second, exp);
    }
}
