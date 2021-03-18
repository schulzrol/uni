/**
 * @author xschul06@fit.vutbr.cz
 */
package ija.ija2020.homework1.store;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;

import java.time.LocalDate;

public class StoreGoodsItem implements GoodsItem {
    public Goods goodsType;
    public LocalDate loaded;

    @Override
    public Goods goods() {
        return this.goodsType;
    }

    public StoreGoodsItem(Goods goods, LocalDate loaded) {
        this.goodsType = goods;
        this.loaded = loaded;
    }

    @Override
    public boolean sell(){
        return this.goodsType.remove(this);
    }
}
