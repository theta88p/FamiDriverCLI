#pragma once

#include "MMLReader.h"

class FileWriter
{
private:
	std::wstring outputPath;
	std::string title;
	std::string artist;
	std::string copyright;
	int musicnum = 0;
	int dpcmoffset = 0;
	int expdevice = 0;
	std::map<int, DpcmInfo> dpcmlist;
	std::vector<unsigned char> seqdata;

public:
	FileWriter();
	FileWriter(std::wstring& path, MMLReader& mmlreader);

	void createBin();
	void createNes();
	void createFds();
	void createNsf();
};

