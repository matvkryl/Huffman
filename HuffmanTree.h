#pragma once

#include <unordered_map>
#include <string>
#include <fstream>

class HuffmanTree {
private:
	struct HuffmanNode;

	HuffmanNode* root;
	std::unordered_map<uint8_t, std::pair<uint64_t, uint8_t>> codes;

	HuffmanNode* buildTree(int* frequency);
	void deleteNode(HuffmanNode* cur);

	void calcCodes(HuffmanNode* node, uint64_t curCode, uint8_t length);
	int* calcFrequency(std::string fileName);

	void writeFrequency(std::ofstream& fOut, int* frequency);
	int* readFrequency(std::ifstream& fInp);

	void printFrequency(int* frequency);
	void printNode(HuffmanNode* cur, int depth);
public:
	HuffmanTree();
	void printTree();
	void calcCodes();
	bool encodeFile(std::string fileNameInput = "files\\input.txt", std::string fileNameOutput = "files\\input.dat");
	bool decodeFile(std::string fileNameInput = "files\\input.dat", std::string fileNameOutput = "files\\output.txt");
	~HuffmanTree();
};

