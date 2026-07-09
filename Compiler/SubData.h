#pragma once
#include <vector>

struct SubData
{
	int num = 0;
	int addr = 0;
	int tone = 0; // 抜けたときの音色
	std::vector<unsigned char>data;
};

