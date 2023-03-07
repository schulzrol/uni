package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.Maze;
import ija.ija2022.homework1.common.MazeObject;

public class WallField implements Field {
    int row, col;
    Maze maze;
    public WallField(int row, int col){
        this.row = row;
        this.col = col;
        this.maze = null;
    }
    @Override
    public boolean canMove() {
        return false;
    }

    @Override
    public MazeObject get() {
        return null;
    }

    @Override
    public boolean isEmpty() {
        return true;
    }

    @Override
    public Field nextField(Direction dirs) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean put(MazeObject object) {
        throw new UnsupportedOperationException();
    }

    @Override
    public boolean remove(MazeObject object) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void setMaze(Maze maze) {
        this.maze = maze;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        WallField wallField = (WallField) o;

        if (row != wallField.row) return false;
        return col == wallField.col;
    }

    @Override
    public int hashCode() {
        int result = row;
        result = 31 * result + col;
        return result;
    }
}
