package markup;

import java.util.List;

public class OrderedList extends AbsList{

    public OrderedList(List<ListItem> items) {
        super(items, "enumerate");
    }
}
