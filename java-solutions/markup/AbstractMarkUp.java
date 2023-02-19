package markup;

import java.util.List;

public abstract class AbstractMarkUp implements Mark {
    List<Mark> markUps;
    String mark = "";
    String envType;


    protected AbstractMarkUp(List<Mark> markUps, String envType) {
        this.markUps = markUps;
        this.mark = mark;
        this.envType = envType;
    }

    public void toMarkdown(StringBuilder s) {
        s.append(this.mark);
        for (Mark mark : markUps) {
            mark.toMarkdown(s);
        }
        s.append(this.mark);
    }

    public void toTex(StringBuilder s) {
        s.append("\\").append(envType).append("{");
        for (Mark mark : markUps) {
            mark.toTex(s);
        }
        s.append("}");
    }
}
