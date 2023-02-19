package markup;

public interface Mark {
    void toMarkdown(StringBuilder s);

    void toTex(StringBuilder s);
}
