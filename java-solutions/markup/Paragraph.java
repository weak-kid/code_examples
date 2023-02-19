package markup;

import java.util.List;

public class Paragraph implements Item {

    List<Mark> paragraph;

    public Paragraph(List<Mark> paragraph) {
        this.paragraph = paragraph;
    }

    public void toMarkdown(StringBuilder s) {
        for (Mark mark : paragraph) {
            mark.toMarkdown(s);
        }
    }

    public void toTex(StringBuilder s) {
        for (Mark mark : paragraph) {
            mark.toTex(s);
        }
    }
}
