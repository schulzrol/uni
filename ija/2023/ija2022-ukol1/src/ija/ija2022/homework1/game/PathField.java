package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.Maze;
import ija.ija2022.homework1.common.MazeObject;

public class PathField implements Field {
    int row, col;
    MazeObject objectOnTop;
    Maze maze;
    public PathField(int row, int col){
        this.row = row;
        this.col = col;
        this.objectOnTop = null;
        this.maze = null;
    }
    @Override
    public boolean canMove() {
        return true;
    }

    @Override
    public MazeObject get() {
        return this.objectOnTop;
    }

    @Override
    public boolean isEmpty() {
        return this.get() == null;
    }

    @Override
    public Field nextField(Direction dirs) {
        if (this.maze == null) return null;

        return this.maze.getField(this.row + dirs.deltaRow(), this.col + dirs.deltaCol());
    }

    @Override
    public boolean put(MazeObject object) {
        this.objectOnTop = object;
        return true;
    }

    @Override
    public boolean remove(MazeObject object) {
        if (this.isEmpty() || this.get() != object) return false;
        this.objectOnTop = null;
        return true;
    }

    @Override
    public void setMaze(Maze maze) {
        this.maze = maze;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        PathField pathField = (PathField) o;

        if (row != pathField.row) return false;
        return col == pathField.col;
    }

    @Override
    public int hashCode() {
        int result = row;
        result = 31 * result + col;
        return result;
    }
}
