package markup;

public class Text implements Mark {
    String text;

    public Text(String text) {
        this.text = text;
    }

    public void toMarkdown(StringBuilder s) {
        s.append(this.text);
    }


    public void toTex(StringBuilder s) {
        s.append(this.text);
    }
}
