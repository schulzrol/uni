package ija.ija2022.homework1.common;

public interface Maze {
    Field getField(int row, int col);
    int numCols();
    int numRows();
}
