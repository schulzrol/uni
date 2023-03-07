package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.Maze;

class MazeMap implements Maze{

    private Field[][] map;
    private int cols;
    private int rows;
    public MazeMap(Field[][] linesAsFields){
        this.map = linesAsFields;
        this.rows = linesAsFields.length;
        this.cols = linesAsFields[0].length;

        for (Field[] row : this.map){
            for (Field mo : row){
                mo.setMaze(this);
            }
        }
    }
    @Override
    public Field getField(int row, int col) {
        return this.map[row][col];
    }

    @Override
    public int numCols() {
        return this.cols;
    }

    @Override
    public int numRows() {
        return this.rows;
    }
}