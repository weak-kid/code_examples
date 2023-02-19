package markup;

import java.util.List;

public class UnorderedList extends AbsList {
    public UnorderedList(List<ListItem> items) {
        super(items, "itemize");
    }
}