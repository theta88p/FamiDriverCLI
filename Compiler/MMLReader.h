#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <queue>
#include <filesystem>

#include "TrackData.h"
#include "EnvData.h"
#include "SubData.h"
#include "DpcmInfo.h"
#include "Utils.h"
#include "NoteMapDif.h"
#include "cmpByStringLength.h"

#define REST_DEFLEN			0x6c
#define LOOP_START			0x6d
#define LOOP_END			0x6e			
#define LOOP_MID_END		0x6f
#define GATE_TIME			0x70
#define KEYSHIFT_REL		0x71
#define KEYSHIFT_ABS		0x72
#define TAI_SLUR			0x73
#define TONE				0x74
#define TEMPO				0x75
#define PLAY_DATA			0x76
#define VOLUME_ENV			0x77
#define VOLUME_ENV_STOP		0x78
#define FREQ_ENV			0x79
#define FREQ_ENV_STOP		0x7a
#define NOTE_ENV			0x7b
#define NOTE_ENV_STOP		0x7c
#define TONE_ENV			0x7d
#define TONE_ENV_STOP		0x7e
#define TRACK_END			0x7f

#define REST_LENGTH			0xec
#define INF_LOOP			0xed
#define DEF_LENGTH			0xee
#define VOLUME_REL			0xef
#define VOLUME_ABS			0xf0
#define DETUNE				0xf1
#define HW_SWEEP			0xf2
#define HW_ENV				0xf3
#define SW_SWEEP			0xf4
#define SW_SWEEP_STOP		0xf5
#define ENV_DISABLE			0xf6
#define MEM_WRITE			0xf7
#define SUBROUTINE			0xf8
#define FDS_MOD_FREQ		0xf9
#define FDS_MOD_TONE		0xfa
#define FDS_MOD_ENV			0xfb

#define DEV_2A03_SQR1		0
#define DEV_2A03_SQR2		1
#define DEV_2A03_TRI		2
#define DEV_2A03_NOISE		3
#define DEV_2A03_DPCM		4
#define DEV_VRC6_SQR1		5
#define DEV_VRC6_SQR2		6
#define DEV_VRC6_SAW		7
#define DEV_MMC5_SQR1		8
#define DEV_MMC5_SQR2		9
#define DEV_SS5B_SQR1		10
#define DEV_SS5B_SQR2		11
#define DEV_SS5B_SQR3		12
#define DEV_FDS				13

enum ExtDev
{
	VRC6 = 1,
	VRC7 = 2,
	FDS = 4,
	MMC5 = 8,
	N163 = 16,
	SS5B = 32,
};

class MMLReader
{
private:
	std::stringstream ss;
	std::wstring inputPath;
	std::map<int, EnvData> envdata;
	std::map<int, SubData> subdata;
	std::map<int, NoteMapDif> mapdiflist;
	std::vector<unsigned char>lengthtbl;
	bool octaveReverse = false;
	int v_offset;
	int f_offset;
	int n_offset;
	int t_offset;
	int linenum;
	int deflen;
	int timebase;
	int totalpos;

	bool skipUntil(std::string input);
	bool isDigit(char c);
	bool isHex(char c);
	bool skipSpace();
	bool skipComment();
	bool skipSpaceUntilNextLine();
	bool findStr(std::string str);
	bool findStr(std::string str, std::string exclude);
	bool isNextChar(char c);
	bool isNextStr(std::string str);
	bool getc(char& c);
	void syntaxErr();
	bool getMultiDigit(int& d);
	bool getMultiHex(int& d);
	bool getStrInQuote(std::string& str);
	bool getByte(int& h);
	void calcLength(char cmd, std::vector<unsigned char>& data, std::vector<unsigned char> tbl, int& grace, int shorten);
	void pushEnvAssign(std::vector<unsigned char>& data, std::map<int, EnvData>& envdatalist, int envtype, int envnum, int delay, bool ismap, int offset);
	void getAndPushEnvAssign(std::vector<unsigned char>& data, std::map<int, EnvData>& envdatalist, int envtype, int offset);
	void makeLengthTbl(std::vector<unsigned char> &tbl);
	void readMacro(std::map<std::string, std::string, cmpByStringLength>& macrolist);
	void replaceMacro(std::map<std::string, std::string, cmpByStringLength>& macrolist, std::stringstream& ss);
	void readDifinitions();
	void readSubRoutine(int& subsize);
	void readEnvelope(int& envsize);
	void readBrackets(int startpos, int trackheadsize, std::vector<unsigned char>& trhead, std::vector<unsigned char>& trbody, int& tone);
	void readWaveData(std::vector<unsigned char>& out);
	void readModData(std::vector<unsigned char>& out);
	void getCmdArgs(CommandArgs& args);
	bool getNoteNumber(int& nn);

public:
	std::string title;
	std::string artist;
	std::string copyright;
	std::vector<int> musiclist;			//曲番号とn曲目の対応リスト。呼び出し時に逆引きする。
	std::map<int, DpcmInfo> dpcmlist;
	std::vector<unsigned char> seqdata;
	int dpcmoffset;
	int extdevice;

	MMLReader();
	MMLReader(std::wstring& input);
	~MMLReader();

	void readMML();
};

