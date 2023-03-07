package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.MazeObject;

public class PacmanObject implements MazeObject {
    Field standsOn;
    public PacmanObject(PathField standsOn){
        this.standsOn = standsOn;
    }
    @Override
    public boolean canMove(Field.Direction dir) {
        return this.standsOn.nextField(dir).canMove();
    }

    @Override
    public boolean move(Field.Direction dir) {
        if (this.canMove(dir)) {
            this.standsOn.remove(this);
            this.standsOn = this.standsOn.nextField(dir);
            this.standsOn.put(this);
            return true;
        }
        return false;
    }
}
