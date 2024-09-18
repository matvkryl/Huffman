#include <iostream>

#include "HuffmanTree.h"

using namespace std;

int main() {
	HuffmanTree huffman;
	if (huffman.encodeFile("files\\War and piece.txt", "files\\War and piece.dat"))
		cout << "encoding succeeded" << endl;
	else
		cout << "encoding failed" << endl;
	if (huffman.decodeFile("files\\War and piece.dat", "files\\War and piece1.txt"))
		cout << "decoding succeeded" << endl;
	else
		cout << "decoding failed" << endl;
	return 0;
}