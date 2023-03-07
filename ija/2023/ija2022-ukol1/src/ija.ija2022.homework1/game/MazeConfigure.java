package ija.ija2022.homework1.game;

import ija.ija2022.homework1.common.Field;
import ija.ija2022.homework1.common.Maze;

public class MazeConfigure {
    Field[][] linesAsFields;
    int rowCount;
    int rows, cols;
    boolean canRead;
    boolean failed;
    public MazeConfigure(){
        this.canRead = true;
        this.failed = false;
        this.linesAsFields = null;
        this.rowCount = 0;
        this.rows=0;
        this.cols=0;
    }

    public boolean processLine(String line){
        if (!this.canRead) return false;
        if ((this.rowCount + 1 > this.rows) || (this.cols != line.length())){
            this.stopReading();
            return false;
        }

        for (int colCount = 0; colCount<line.length(); colCount++){
            switch (line.charAt(colCount)) {
                case '.' -> this.linesAsFields[this.rowCount][colCount] = new PathField(this.rowCount, colCount);
                case 'X' -> this.linesAsFields[this.rowCount][colCount] = new WallField(this.rowCount, colCount);
                case 'S' -> {
                    PathField pacmanStartField = new PathField(this.rowCount, colCount);
                    PacmanObject pacman = new PacmanObject(pacmanStartField);
                    pacmanStartField.put(pacman);
                    this.linesAsFields[this.rowCount][colCount] = pacmanStartField;
                }
                default -> {
                    this.stopReading();
                    this.failed = true;
                    return false;
                }
            }
        }
        return true;
    }

    public void startReading(int rows, int cols){
        this.linesAsFields = new Field[rows][cols];
    }

    public boolean stopReading(){
        this.canRead = false;
        return true;
    }

    public Maze createMaze() {
        if (this.failed) return null;
        else return new MazeMap(this.linesAsFields);
    }
}
