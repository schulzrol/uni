/**
 * @author xschul06@fit.vutbr.cz
 */
package ija.ija2020.homework1.store;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;
import ija.ija2020.homework1.goods.GoodsShelf;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class StoreShelf implements GoodsShelf {

    private Map<Goods, List<GoodsItem>> items = new HashMap();

    public StoreShelf(){}

    @Override
    public void put(GoodsItem item) {
        Goods goods = item.goods();
        if (this.items.containsKey(goods)) {
            ((List)this.items.get(goods)).add(item);
        } else {
            List<GoodsItem> list = new ArrayList();
            list.add(item);
            this.items.put(goods, list);
        }
    }

    @Override
    public boolean containsGoods(Goods goods) {
        List<GoodsItem> list = (List)this.items.get(goods);
        return list == null ? false : !list.isEmpty();
    }

    @Override
    public GoodsItem removeAny(Goods goods) {
        List<GoodsItem> list = (List)this.items.get(goods);
        if (list == null) {
            return null;
        } else {
            return list.isEmpty() ? null : (GoodsItem) list.remove(0);
        }
    }

    @Override
    public int size(Goods goods) {
        List<GoodsItem> list = (List)this.items.get(goods);
        return list == null ? 0 : list.size();
    }
}
