package markup;

import java.util.List;

public class ListItem {
    List<Item> listItem;

    public ListItem(List<Item> listItem) {
        this.listItem = listItem;
    }

    public void toTex(StringBuilder s) {
        s.append("\\item ");
        for (Item item : listItem) {
            item.toTex(s);
        }
    }
}
