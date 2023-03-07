package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.Maze;

public class MazeConfigure {
    Field[][] linesAsFields;
    int rowCount;
    int rows, cols;
    boolean failed;
    public MazeConfigure(){
        this.failed = false;
        this.linesAsFields = null;
        this.rowCount = 0;
        this.rows=0;
        this.cols=0;
    }

    private Field fieldFactory(char fieldSymbol, int row, int col) {
        switch (fieldSymbol) {
            case '.' -> {return new PathField(row, col);}
            case 'X' -> {return new WallField(row, col);}
            case 'S' -> {
                        PathField pacmanStartField = new PathField(row, col);
                        pacmanStartField.put(new PacmanObject(pacmanStartField));
                        return pacmanStartField;
                    }
            default -> {
                return null;
            }
        }
    }

    public boolean processLine(String line){
        Field[] newRow = new Field[this.cols];
        // add left and right boundary walls
        newRow[0] = new WallField(this.rowCount, 0);
        newRow[this.cols-1] = new WallField(this.rowCount, this.cols-1);

        for (int i=0; i<line.length(); i++){
            char fieldSymbol = line.charAt(i);
            Field newField = fieldFactory(fieldSymbol, this.rowCount, i+1);
            if (newField == null) {this.failed = true; return false;}
            newRow[i+1] = newField;
        }
        this.linesAsFields[this.rowCount] = newRow;
        this.rowCount++;
        return true;
    }

    public void startReading(int rows, int cols){
        this.rows = rows + 2;
        this.cols = cols + 2;
        this.linesAsFields = new Field[this.rows][this.rows];
        // set first row as boundary walls
        this.linesAsFields[0] = lineOfWalls(this.cols, 0);
        this.rowCount++;
    }

    private WallField[] lineOfWalls(int count, int row){
        WallField[] walls = new WallField[count];
        for (int ci=0; ci<count; ci++){
            walls[ci] = new WallField(row, ci);
        }
        return walls;
    }

    public boolean stopReading(){
        this.linesAsFields[this.rows-1] = lineOfWalls(this.cols, this.rows-1);
        return true;
    }

    public Maze createMaze() {
        if (this.failed) return null;
        else {
            return new MazeMap(this.linesAsFields);
        }
    }
}
