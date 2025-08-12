#pragma once
#include <vector>

struct SubData
{
	int num;
	int addr;
	int tone; // 抜けたときの音色
	std::vector<unsigned char>data;
};

