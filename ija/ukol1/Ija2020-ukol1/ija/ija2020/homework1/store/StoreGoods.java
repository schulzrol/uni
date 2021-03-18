/**
 * @author xschul06@fit.vutbr.cz
 */
package ija.ija2020.homework1.store;

import ija.ija2020.homework1.goods.Goods;
import ija.ija2020.homework1.goods.GoodsItem;
import ija.ija2020.homework1.goods.GoodsShelf;

import ija.ija2020.homework1.store.StoreGoodsItem;

import java.util.List;
import java.util.ArrayList;
import java.time.LocalDate;
import java.util.Objects;

public class StoreGoods implements Goods{
    private String name;
    private List<GoodsItem> items = new ArrayList<GoodsItem>();

    public StoreGoods(String name) {
        this.name = name;
    }

    @Override
    public boolean equals(Object obj){
        if (obj == null) return false;
        if(!(obj instanceof StoreGoods)) return false;

        StoreGoods g = (StoreGoods) obj;
        return this.name.equals(g.getName());
    }

    @Override
    public int hashCode(){
        return Objects.hash(this.getName());
    }

    @Override
    public boolean addItem(GoodsItem item){
        return this.items.add(item);
    }

    @Override
    public boolean empty(){
        return this.items.isEmpty();
    }

    @Override
    public String getName(){
        return this.name;
    }

    @Override
    public GoodsItem newItem(LocalDate loaded){
        GoodsItem newItem = new StoreGoodsItem(this, loaded);
        this.addItem(newItem);
        return newItem;
    }

    @Override
    public boolean remove(GoodsItem item){
        int pos = this.items.indexOf(item);
        if (pos == -1) {
            return false;
        }
        else {
            this.items.remove(pos);
            return true;
        }
    }

    @Override
    public int size(){
        return this.items.size();
    }
}
