
SRC="."

rm -rf build
mkdir build

javac -classpath ${SRC}:junit.jar:ija2020-ukol1.jar -d build ${SRC}/ija/ija2020/homework1/Homework1Test.java

java -cp junit.jar:ija2020-ukol1.jar org.junit.runner.JUnitCore ija.ija2020.homework1.Homework1Test

