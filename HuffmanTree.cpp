#include <queue>
#include <vector>
#include <iostream>

#include "HuffmanTree.h"

#ifndef FREQUENCY_SIZE
#define FREQUECNY_SIZE 256
#endif

struct HuffmanTree::HuffmanNode {
	uint8_t character;
	int frequency;
	HuffmanNode* left;
	HuffmanNode* right;
	HuffmanNode(uint8_t character, int frequency, HuffmanNode* left, HuffmanNode* right) : character(character), frequency(frequency), left(left), right(right) {}
};

HuffmanTree::HuffmanNode* HuffmanTree::buildTree(int* frequency) {
	auto cmp = [](HuffmanNode* left, HuffmanNode* right) {return left->frequency > right->frequency; };
	std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(cmp)> treeNodes(cmp);
	for (int i = 0; i < FREQUECNY_SIZE; i++) {
		if (frequency[i])
			treeNodes.push(new HuffmanNode(uint8_t(i), frequency[i], nullptr, nullptr));
	}
	if (treeNodes.size() == 0)
		return nullptr;
	while (treeNodes.size() > 1) {
		HuffmanNode* left = treeNodes.top(); treeNodes.pop();
		HuffmanNode* right = treeNodes.top();	treeNodes.pop();

		treeNodes.push(new HuffmanNode(0, left->frequency + right->frequency, left, right));
	}
	return treeNodes.top();
}

void HuffmanTree::deleteNode(HuffmanNode* cur) {
	if (cur->left != cur->right) {
		deleteNode(cur->left);
		deleteNode(cur->right);
	}
	delete cur;
}

void HuffmanTree::calcCodes(HuffmanNode* node, uint64_t curCode, uint8_t length) {
	if (node->left == node->right)
		codes[node->character] = std::make_pair(curCode, length);
	else {
		calcCodes(node->left, curCode << 1, length + 1);
		calcCodes(node->right, curCode << 1 | 1, length + 1);
	}
}

int* HuffmanTree::calcFrequency(std::string fileName) {
	std::ifstream fInp(fileName, std::ios::binary);
	if (!fInp) {
		return nullptr;
	}
	int* frequency = new int[FREQUECNY_SIZE];
	for (int i = 0; i < FREQUECNY_SIZE; i++)
		frequency[i] = 0;
	for (uint8_t c; fInp.read((char*)&c, sizeof(c));) {
		++frequency[c];
	}
	fInp.close();
	return frequency;
}

void HuffmanTree::writeFrequency(std::ofstream& fOut, int* frequency) {
	if (!fOut.write((char*)frequency, sizeof(frequency[0]) * FREQUECNY_SIZE)) {
		return;
	}
	fOut.flush();
}

int* HuffmanTree::readFrequency(std::ifstream& fInp) {
	int* frequency = new int[FREQUECNY_SIZE];
	if (!fInp.read((char*)frequency, sizeof(frequency[0]) * FREQUECNY_SIZE)) {
		delete[] frequency;
		return nullptr;
	}
	return frequency;
}

void HuffmanTree::printFrequency(int* frequency) {
	for (int i = 0; i < FREQUECNY_SIZE; i++) {
		if (frequency[i])
			std::cout << frequency[i] << std::endl;
	}
}

void HuffmanTree::printNode(HuffmanNode* cur, int depth) {
	if (cur->left != cur->right) {
		printNode(cur->right, depth + 4);
		for (int i = 0; i < depth; i++) std::cout << " ";
		std::cout << cur->frequency << '\n';
		printNode(cur->left, depth + 4);
	}
	else {
		for (int i = 0; i < depth; i++) std::cout << " ";
		std::cout << cur->frequency << '\n';
	}
}


HuffmanTree::HuffmanTree() {
	root = nullptr;
}

void HuffmanTree::printTree() {
	printNode(root, 0);
}

void HuffmanTree::calcCodes() {
	codes.clear();
	calcCodes(root, 0, 0);
}

bool HuffmanTree::encodeFile(std::string fileNameInput, std::string fileNameOutput) {
	int* frequency = calcFrequency(fileNameInput);
	if (!frequency) {
		return false;
	}

	root = buildTree(frequency);
	//printTree();

	calcCodes();

	uint32_t degrees[32];
	degrees[0] = 1;
	for (int i = 1; i < 32; i++) {
		degrees[i] = degrees[i - 1] << 1;
	}

	std::ifstream fInp(fileNameInput, std::ios::binary);
	if (!fInp) {
		return false;
	}

	std::vector<uint8_t> encodedBytes;
	uint64_t currentByte = 0;
	uint8_t currentLength = 0;
	for (uint8_t c; fInp.read((char*)&c, sizeof(c));) {
		uint64_t code = codes[c].first;
		uint8_t length = codes[c].second;
		while (length) {
			uint8_t missingLength = 0;
			currentLength + length > 8 ? missingLength = 8 - currentLength : missingLength = length;
			currentByte = currentByte << missingLength | (code / degrees[length - missingLength]);
			currentLength += missingLength;
			if (currentLength == 8) {
				encodedBytes.push_back(currentByte);
				currentLength = 0;
				currentByte = 0;
			}
			code %= degrees[length - missingLength];
			length -= missingLength;
		}
	}
	fInp.close();

	if (currentLength) {
		currentByte <<= 8 - currentLength;
		encodedBytes.push_back(currentByte);
	}

	std::ofstream fOut(fileNameOutput, std::ios::binary);
	if (!fOut) {
		return false;
	}

	fOut.write((char*)&currentLength, sizeof(currentLength));
	writeFrequency(fOut, frequency);

	for (auto& cur : encodedBytes) {
		fOut.write((char*)&cur, sizeof(cur));
	}
	fOut.close();
	delete[] frequency;
	return true;
}

bool HuffmanTree::decodeFile(std::string fileNameInput, std::string fileNameOutput) {
	std::ifstream fInp(fileNameInput, std::ios::binary);
	if (!fInp) {
		return false;
	}

	uint8_t lenOfLastByte = 0;
	fInp.read((char*)&lenOfLastByte, sizeof(lenOfLastByte));

	int* frequency = readFrequency(fInp);

	buildTree(frequency);
	//printTree();

	std::vector<uint8_t> decodedBytes;
	for (uint8_t c; fInp.read((char*)&c, sizeof(c));) {
		decodedBytes.push_back(c);
	}
	fInp.close();

	std::ofstream fOut(fileNameOutput, std::ios::binary);
	if (!fOut) {
		return false;
	}

	uint8_t degrees[8];
	degrees[0] = 1;
	for (int i = 1; i < 8; i++) {
		degrees[i] = degrees[i - 1] << 1;
	}

	HuffmanNode* cur = root;
	for (int i = 0; i < decodedBytes.size() - 1; i++) {
		uint8_t curByte = decodedBytes[i];
		for (int i = 7; i >= 0; i--) {
			if (curByte / degrees[i])
				cur = cur->right;
			else
				cur = cur->left;
			curByte %= degrees[i];
			if (cur->left == cur->right) {
				fOut.write((char*)&(cur->character), sizeof(cur->character));
				cur = root;
			}
		}
	}

	decodedBytes[decodedBytes.size() - 1] >>= 8 - lenOfLastByte;
	for (int i = lenOfLastByte - 1; i >= 0; i--) {
		if (decodedBytes[decodedBytes.size() - 1] / degrees[i])
			cur = cur->right;
		else
			cur = cur->left;
		decodedBytes[decodedBytes.size() - 1] %= degrees[i];
		if (cur->left == cur->right) {
			fOut.write((char*)&(cur->character), sizeof(cur->character));
			cur = root;
		}
	}
	fOut.close();
	delete[] frequency;
}

HuffmanTree::~HuffmanTree() {
	deleteNode(root);
}

