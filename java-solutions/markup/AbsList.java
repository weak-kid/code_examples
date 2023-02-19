package markup;

import java.util.ArrayList;
import java.util.List;

abstract class AbsList implements Item {
    public List<ListItem> items;
    public String env;

    protected AbsList(List<ListItem> items, String env) {
        this.env = env;
        this.items = items;
    }

    public void toTex(StringBuilder s) {
        s.append("\\begin{").append(env).append("}");
        for (ListItem item : items) {
            item.toTex(s);
        }
        s.append("\\end{").append(env).append("}");
    }
}