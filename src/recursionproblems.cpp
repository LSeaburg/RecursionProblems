// This is the .cpp file you will edit and turn in.
// We have provided a skeleton for you,
// but you must finish it as described in the spec.
// Also remove these comments here and add your own.
// TODO: remove this comment header

// Please feel free to add any other #includes you need!
#include "recursionproblems.h"
#include <cmath>
#include <iostream>
#include "hashmap.h"
#include "map.h"
#include "random.h"
using namespace std;

void readGrammerFile(istream& input, HashMap<string, Vector<string> >& grammerMap);
string grammerHelper(string sentencePart, HashMap<string, Vector<string> >& grammerMap);

/* If Karel is directly above or to the side, there is only one path.
 * Otherwise, the total number of paths is the sum of both squares
 * next to her.
 */
int countKarelPaths(int street, int avenue) {
    int paths = 0;
    if (street < 1 || avenue < 1) {
        throw "illegal street or avenue!";
    }
    if (street == 1 || avenue == 1) { //base case
        paths = 1;
    } else { //recursion case
        paths = countKarelPaths(street - 1, avenue) + countKarelPaths(street, avenue - 1);
    }
    return paths;
}

/* Will take care of negitive signs at the beginning of the first one,
 * then handels the last character one at a time, passing the rest
 * and multiplying by 10
 */
int convertStringToInteger(string exp) {
    if (exp.length() == 1) { //base case
        char ch = (char)exp[0];
        if ((int)ch - 48 < 10 && (int)ch - 48 > -1) {
            return (int)ch - 48; //based of ascii values
        }
        throw "illegal format";
    }
    if (exp[0] == '-') {
        exp = exp.substr(1, exp.length() - 1);
        if (exp[0] == '-') {
            throw "illegal format.";
        }
        return -1 * convertStringToInteger(exp);
    }
    string lastDigit = exp.substr(exp.length() - 1, 1);
    string restDigits = exp.substr(0, exp.length() - 1);
    return convertStringToInteger(lastDigit) + 10 * convertStringToInteger(restDigits); //recursive case
}

/* This program removes the inmost brackets that are right next to each other and keeps
 * removing them so that more get next to each other. Eventually they will all be eliminated.
 */
bool isBalanced(string exp) {
    if (exp == "") { //base case
        return true;
    } else if (exp.find("()") != -1){
        exp.replace(exp.find("()"), 2, "");
        return isBalanced(exp);
    } else if (exp.find("{}") != -1){
        exp.replace(exp.find("{}"), 2, "");
        return isBalanced(exp);
    } else if (exp.find("[]") != -1){
        exp.replace(exp.find("[]"), 2, "");
        return isBalanced(exp);
    } else if (exp.find("<>") != -1){
        exp.replace(exp.find("<>"), 2, "");
        return isBalanced(exp);
    }
    return false;
}

/* Will store the weight of the person as their weight plus the weight
 * of all the people above them. This process continues until the top of the
 * pyramid is reached.
 */
double weightOnKnees(int row, int col, Vector<Vector<double> >& weights) {
    if (row == 0 && col == 0) { //base case
        return weights[0][0];
    }
    double weight = weights[row][col];
    if (col != weights[row - 1].size() ) {
        weight += 0.5 * weightOnKnees(row - 1, col, weights);
    }
    if (col != 0) {
        weight += 0.5 * weightOnKnees(row - 1, col - 1, weights);
    }
    return weight;
}

/* This triangle draws 1 triangle, and then draws 3 Sierpinski triangles, each of 1 less order
 * in each of the corners of the first triangle.
 */
void drawSierpinskiTriangle(GWindow& gw, double x, double y, double size, int order) {
    if (x < 0 || y < 0 || size < 0 || order < 0) {
        throw "input not valid";
    } else if (order == 0) {
        return;
    }
    gw.drawLine(x, y, x + size, y);
    gw.drawLine(x, y, x + size / 2, y + (sqrt(3) * size / 2));
    gw.drawLine(x + size, y, x + size / 2, y + (sqrt(3) * size / 2));
    drawSierpinskiTriangle(gw, x, y, size / 2, order - 1);
    drawSierpinskiTriangle(gw, x + size / 2, y, size / 2, order - 1);
    drawSierpinskiTriangle(gw, x + size / 4, y + (sqrt(3) * size / 4), size / 2, order - 1);
}

/* This program saves the color of the current pixel and crawls out, searching
 * for the pixels around it that are the same color. If so it will color them; if not
 * it will back track to the last pixel and check that for different paths.
 */
int floodFill(GBufferedImage& image, int x, int y, int color) {
    if (!image.inBounds(x, y)) {
        throw "out of bounds";
    }
    int curColor = image.getRGB(x, y);
    if (color == curColor) {
        return 0;
    }
    int changed = 1;
    image.setRGB(x, y, color);
    if (image.inBounds(x - 1, y) && image.getRGB(x - 1, y) == curColor) {
        changed += floodFill(image, x - 1, y, color);
    }
    if (image.inBounds(x + 1, y) && image.getRGB(x + 1, y) == curColor) {
        changed += floodFill(image, x + 1, y, color);
    }
    if (image.inBounds(x, y - 1) && image.getRGB(x, y - 1) == curColor) {
        changed += floodFill(image, x, y - 1, color);
    }
    if (image.inBounds(x, y + 1) && image.getRGB(x, y + 1) == curColor) {
        changed += floodFill(image, x, y + 1, color);
    }
    return changed;
}

/* This calls the function that generates the map, and then calls the sentence
 * part creation function the desired number of times.
 */
Vector<string> grammarGenerate(istream& input, string symbol, int times) {
    HashMap<string, Vector<string> > grammerMap;
    Vector<string> v;
    readGrammerFile(input, grammerMap);
    if (!grammerMap.containsKey(symbol)) {
        throw "invalid sentence part";
    }
    for (int i = 0; i < times; i++) {
        v.add(trim(grammerHelper(symbol, grammerMap) ) );
    }
    return v;
}

/* Stores all of the words in a map, putting the text on the left side of the
 * ::= as the key to acess a vector of the possible endings. It throws an exception
 * if the same thing is defined twice in the file.
 */
void readGrammerFile(istream& input, HashMap<string, Vector<string> >& grammerMap) {
    string line;
    Vector<string> temp;
    while(getline(input, line)) {
        temp = stringSplit(line, "::=");
        if (grammerMap.containsKey(temp[0])) {
            throw "multiple non-terminal definitions";
        }
        grammerMap.add(temp[0],  stringSplit(temp[1],  "|"));
    }
    return;
}

/* Generates the part of word that is input. If the part is a non-terminal, it will
 * select a random word and add that to the sentence part that is being generated.
 * If it is a terminal, it will recall the function on each part of the terminal definition.
 */
string grammerHelper(string sentencePart, HashMap<string, Vector<string> >& grammerMap) {
    string temp = "";
    Vector<string> phraseParts;
    if (!grammerMap.containsKey(sentencePart)) { //base case
        return sentencePart + " ";
    }
    string select = trim(randomElement(grammerMap[sentencePart]) ); //recursive case
    phraseParts = stringSplit(select, " ");
    for (string part : phraseParts) { //check for every word in the description
        temp += grammerHelper(trim(part), grammerMap);
    }
    return temp;
}

bool reverseGrammerSolver(string sentence, HashMap<string, Vector<string> >& grammerMap) {
    Vector<string> sentenceParts;
}
