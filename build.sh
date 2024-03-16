#!/bin/bash

rm src/jradicom/*.class
rm src/example/*.class

javac src/jradicom/JRadicom.java src/example/Main.java

javac -h src/jradicom src/jradicom/JRadicom.java
make clean
make