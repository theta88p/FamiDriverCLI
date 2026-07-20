#include "MMLReader.h"

#include <numeric>

//ノートテーブル（ノート文字と音階の関係）
                      /* a b  c d e f g */
const unsigned char notetbl[] = { 9, 11, 0, 2, 4, 5, 7 };
//音長テーブル（音長番号と192/n音符の関係）
/* 1 2 3 4 6 8 12 16 24 32 48 64 */
const unsigned char nthnotetbl[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96 };

//ソフトウェアスイープのディレイテーブル（フレーム単位）
static const unsigned char sweepdelaytbl[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    8, 9, 10, 11, 12, 13, 14, 15,
    16, 18, 20, 22, 24, 28, 32, 36,
    40, 48, 56, 64, 80, 96, 128, 255
};

static bool packSoftwareSweep(int start, int end, int delay, int speed, unsigned char& arg1, unsigned char& arg2)
{
    int pitch = end - start;
    if (pitch < -64 || pitch > 63 || speed == 0 || speed < -7 || speed > 7)
    {
        return false;
    }

    int delayIndex = -1;
    for (int i = 0, size = static_cast<int>(sizeof(sweepdelaytbl)); i < size; i++)
    {
        if (sweepdelaytbl[i] == delay)
        {
            delayIndex = i;
            break;
        }
    }

    if (delayIndex < 0)
    {
        return false;
    }

    arg1 = static_cast<unsigned char>(pitch) & 0x7f;
    if (speed < 0)
    {
        arg1 |= 0x80;
        speed = -speed;
    }
    arg2 = static_cast<unsigned char>((delayIndex << 3) | speed);
    return true;
}

MMLReader::MMLReader()
{
    totalpos = 0;
    linenum = 1;
    deflen = 4;
    timebase = 96;
    dpcmoffset = 0;
    v_offset = 0;
    f_offset = 0;
    n_offset = 0;
    t_offset = 0;
    expdevice = 0;
    n163WaveOffsets.assign(16, 0);
    n163WaveLengthRegs.assign(16, 0xe0);
    n163WaveLengthUnits.assign(16, 8);
}

MMLReader::MMLReader(std::wstring& input) : MMLReader()
{
    inputPath = input;
}

MMLReader::~MMLReader()
{

}


void MMLReader::readMML()
{
    int n = 0;
    int music = 0;
    std::map<std::string, std::string, cmpByStringLength> macrolist;
    std::vector<unsigned char> head;
    std::vector<unsigned char> body;
    std::vector<unsigned char> fds_wavdata;
	std::vector<unsigned char> fds_moddata;
    std::vector<unsigned char> n163_wavdata;
	int fds_wavaddr = 0;
    int fds_modaddr = 0;
    int n163_wavaddr = 0;
    std::ifstream ifs;

    ifs.open(inputPath);

    if (!ifs)
    {
        std::wcerr << "Can not open '" << inputPath << "'" << std::endl;
        exit(1);
    }
    
    ss << ifs.rdbuf();

    readMacro(macrolist);
    linenum = 1;
    
    ss.clear();
    ss.seekg(0);
    std::stringstream ss2;

    replaceMacro(macrolist, ss2);
    /*
    while (!ss2.eof())
    {
        std::string line;
        std::getline(ss2, line);
        std::wcout << line.c_str() << std::endl;
    }
    */
    linenum = 1;
    ss.str("");
    ss.clear();
    ss << ss2.rdbuf();

    while (!ss.eof())      //最初に曲数だけ取得しておく。ヘッダサイズが決まらないとエンベロープアドレスが取得できないため。
    {
        if (findStr("music"))
        {
            skipSpace();
            if (getMultiDigit(n))
            {
                musiclist.push_back(n);
                music++;
            }
        }
    }

    if (music < 1)
    {
        std::wcerr << "No Music." << std::endl;
        exit(1);
    }

    linenum = 1;

    //曲数 * 曲アドレス2byte
    int musichead = music * 2;
    totalpos = musichead;

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    makeLengthTbl(lengthtbl);
    readDifinitions();

    if (expdevice & Expdev::N163)
    {
        linenum = 1;
        ss.clear();
        ss.seekg(0);
        readN163ChannelCounts();
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す
    
    if (expdevice & Expdev::FDS)
    {
        readWaveData(fds_wavdata);
		std::copy(fds_wavdata.begin(), fds_wavdata.end(), std::back_inserter(body));
        fds_wavaddr = totalpos;
		totalpos += fds_wavdata.size();
        linenum = 1;
        ss.clear();
        ss.seekg(0);    //読み込み位置を戻す
        readModData(fds_moddata);
		std::copy(fds_moddata.begin(), fds_moddata.end(), std::back_inserter(body));
		fds_modaddr = totalpos;
		totalpos += fds_moddata.size();
    }

    if (expdevice & Expdev::N163)
    {
        readWaveData(n163_wavdata);
        n163_wavaddr = totalpos;
        std::copy(n163_wavdata.begin(), n163_wavdata.end(), std::back_inserter(body));
        totalpos += n163_wavdata.size();

        // 波形長と発音数を補正済みの周波数表をシーケンス共通領域に置く。
        // 3bit正規化しておき、ドライバ側はオクターブ分の右シフトだけで済ませる。
        static constexpr unsigned int n163BaseFreq[] =
        {
            0x011f66, 0x01307d, 0x014298, 0x0155c7,
            0x016a19, 0x017fa1, 0x019671, 0x01ae9c,
            0x01c837, 0x01e35a, 0x020016, 0x021e89
        };

        std::vector<unsigned char> uniqueLengthUnits;
        n163FreqTableOffsets.clear();
        auto addLengthUnits = [&](unsigned char lengthUnits)
        {
            if (n163FreqTableOffsets.count(lengthUnits))
            {
                return;
            }
            n163FreqTableOffsets[lengthUnits] = static_cast<int>(uniqueLengthUnits.size()) * 36;
            uniqueLengthUnits.push_back(lengthUnits);
        };

        addLengthUnits(8); // デフォルトの32サンプル波形を各発音数表の先頭に置く
        for (unsigned char lengthUnits : n163WaveLengthUnits)
        {
            addLengthUnits(lengthUnits);
        }

        std::map<int, bool> usedChannelCounts;
        for (const auto& [music, channelCount] : n163ChannelCounts)
        {
            usedChannelCounts[channelCount] = true;
        }

        n163FreqTableBaseAddresses.clear();
        for (const auto& [channelCount, used] : usedChannelCounts)
        {
            n163FreqTableBaseAddresses[channelCount] = totalpos;
            for (unsigned char lengthUnits : uniqueLengthUnits)
            {
                for (unsigned int baseFreq : n163BaseFreq)
                {
                    unsigned int freq = (baseFreq * lengthUnits * channelCount) >> 3;
                    body.push_back(static_cast<unsigned char>(freq));
                    body.push_back(static_cast<unsigned char>(freq >> 8));
                    body.push_back(static_cast<unsigned char>(freq >> 16));
                }
                totalpos += sizeof(n163BaseFreq) / sizeof(n163BaseFreq[0]) * 3;
            }
        }
    }

    if (expdevice & Expdev::VRC7)
    {
        linenum = 1;
        ss.clear();
        ss.seekg(0);
        readVrc7Patch();
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    int envsize = 0;
    readEnvelope(envsize);
    if (expdevice & Expdev::VRC7)
    {
        for (const auto& [k, source] : envdata)
        {
            EnvData reversed = source;
            reversed.addr = totalpos + envsize;
            for (size_t i = 2; i + 1 < reversed.data.size(); i += 2)
            {
                reversed.data[i] = static_cast<unsigned char>(-static_cast<signed char>(reversed.data[i]));
            }
            envsize += static_cast<int>(reversed.data.size());
            vrc7FreqEnvdata[k] = reversed;
        }
    }
    totalpos += envsize;

    for (const auto& [k, v] : envdata)
    {
        std::copy(v.data.begin(), v.data.end(), std::back_inserter(body));
    }
    for (const auto& [k, v] : vrc7FreqEnvdata)
    {
        std::copy(v.data.begin(), v.data.end(), std::back_inserter(body));
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    int subsize = 0;
    readSubRoutine(subsize);
    totalpos += subsize;

    std::vector<SubData> sortedSubdata;
    for (const auto& [k ,v] : subdata)
    {
        sortedSubdata.push_back(v);
    }
    std::sort(sortedSubdata.begin(), sortedSubdata.end(), [](const SubData& left, const SubData& right)
    {
        return left.addr < right.addr;
    });
    for (const auto& v : sortedSubdata)
    {
        std::copy(v.data.begin(), v.data.end(), std::back_inserter(body));
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    auto applyTimebaseAt = [&](int endpos)
    {
        std::string src = ss.str();
        if (endpos < 0 || endpos > static_cast<int>(src.size()))
        {
            endpos = static_cast<int>(src.size());
        }

        int activeTimebase = 96;
        size_t lineStart = 0;
        while (lineStart < static_cast<size_t>(endpos))
        {
            size_t lineEnd = src.find('\n', lineStart);
            if (lineEnd == std::string::npos)
            {
                lineEnd = src.size();
            }

            size_t endLimit = static_cast<size_t>(endpos);
            size_t scanEnd = lineEnd < endLimit ? lineEnd : endLimit;
            std::string line = src.substr(lineStart, scanEnd - lineStart);
            size_t comment = line.find("//");
            if (comment != std::string::npos)
            {
                line.resize(comment);
            }

            size_t pos = 0;
            while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\r'))
            {
                pos++;
            }

            if (pos < line.size() && line[pos] == '#')
            {
                pos++;
                while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
                {
                    pos++;
                }

                const std::string key = "timebase";
                bool match = pos + key.size() <= line.size();
                for (size_t j = 0; match && j < key.size(); j++)
                {
                    match = std::tolower(static_cast<unsigned char>(line[pos + j])) == key[j];
                }

                if (match)
                {
                    pos += key.size();
                    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
                    {
                        pos++;
                    }

                    std::string value;
                    while (pos < line.size() && line[pos] >= '0' && line[pos] <= '9')
                    {
                        value += line[pos];
                        pos++;
                    }

                    if (!value.empty())
                    {
                        activeTimebase = std::atoi(value.c_str());
                    }
                }
            }

            if (lineEnd == src.size())
            {
                break;
            }
            lineStart = lineEnd + 1;
        }

        timebase = activeTimebase;
        lengthtbl.clear();
        makeLengthTbl(lengthtbl);
    };

    for (int i = 0; i < music; i++)
    {
        bool foundMusic = false;

        while (findStr("music"))
        {
            skipSpace();
            if (!(getMultiDigit(n) && n == musiclist[i]))
            {
                continue;
            }

		    std::vector<unsigned char> trhead;
            std::vector<unsigned char> musdata;
            foundMusic = true;
            applyTimebaseAt(static_cast<int>(ss.tellg()));
            if (findStr("{"))
            {
                int seek = (int)ss.tellg();
                int curline = linenum;
                int track = 0;

                while (!ss.eof())                  //トラック数を取得
                {
                    if (findStr("track", "}"))
                    {
                        track++;
                    }
                    else
                    {
                        break;
                    }
                }

                if (track == 0)
                {
                    std::cerr << "No track." << std::endl;
                    exit(1);
                }

                linenum = curline;
                ss.clear();
                ss.seekg(seek);    //読み込み位置を戻す

                //トラック数 x 4byte（トラックのアドレスとトラック番号と音源番号） + トラックヘッダの終端コード + デフォ音長データ
                int trheadsize = track * 4 + 1 + 1;
                if (expdevice & Expdev::FDS)
                {
					trheadsize += 4; //FDSの波形アドレス分
                }
                if (expdevice & Expdev::N163)
                {
                    trheadsize += 6; //N163の発音数、波形データサイズ、波形・周波数表アドレス分
                }

                int tone = 0;
                int n163ChannelCount = (expdevice & Expdev::N163) ? n163ChannelCounts.at(musiclist[i]) : 8;
                readBrackets((int)ss.tellg(), trheadsize, trhead, musdata, tone, n163ChannelCount);
                trhead.push_back(0xff);             //トラックヘッダ終端
                trhead.push_back(lengthtbl[3]);     //デフォルトのデフォルト音長（4分音符）

                if (expdevice & Expdev::FDS)
                {
                    trhead.push_back(fds_wavaddr & 0xff); //FDSの波形アドレス
                    trhead.push_back(fds_wavaddr >> 8);
                    trhead.push_back(fds_modaddr & 0xff); //FDSのモジュレータ波形アドレス
                    trhead.push_back(fds_modaddr >> 8);
                }
                if (expdevice & Expdev::N163)
                {
                    trhead.push_back(n163ChannelCount); //N163の発音数
                    trhead.push_back(n163WaveDataSize); //N163の波形データサイズ
                    trhead.push_back(n163_wavaddr & 0xff); //N163の波形アドレス
                    trhead.push_back(n163_wavaddr >> 8);
                    int freqTableBaseAddress = n163FreqTableBaseAddresses.at(n163ChannelCount);
                    trhead.push_back(freqTableBaseAddress & 0xff); //N163周波数表のベースアドレス
                    trhead.push_back(freqTableBaseAddress >> 8);
                }

                head.push_back(totalpos & 0xff);
                head.push_back(totalpos >> 8);
                totalpos += trhead.size() + musdata.size();
                std::copy(trhead.begin(), trhead.end(), std::back_inserter(body));
                std::copy(musdata.begin(), musdata.end(), std::back_inserter(body));

                linenum = 1;
                ss.clear();
                ss.seekg(0);    //読み込み位置を戻す
            }
            break;
        }

        if (!foundMusic)
        {
            std::cerr << "Music" << musiclist[i] << " is not found." << std::endl;
            exit(1);
        }
    }


    std::copy(head.begin(), head.end(), std::back_inserter(seqdata));
    std::copy(body.begin(), body.end(), std::back_inserter(seqdata));

    ifs.close();
}


void MMLReader::readN163ChannelCounts()
{
    for (int music : musiclist)
    {
        n163ChannelCounts[music] = 8;
    }

    n163MaxChannelCount = 1;
    while (findStr("music"))
    {
        int music;
        skipSpace();
        if (!getMultiDigit(music))
        {
            continue;
        }

        skipSpace();
        if (!isNextChar('{'))
        {
            continue;
        }

        int depth = 1;
        bool n163ChannelCountSpecified = false;
        bool trackFound = false;
        char c;
        while (depth > 0 && ss.get(c))
        {
            if (c == '\n')
            {
                linenum++;
                continue;
            }
            if (c == '/')
            {
                char next;
                if (!ss.get(next))
                {
                    break;
                }
                if (next == '/')
                {
                    skipUntil("\n");
                    linenum++;
                    continue;
                }
                if (next == '*')
                {
                    skipUntil("*/");
                    continue;
                }
                ss.seekg((int)ss.tellg() - 1);
            }
            if (c == '{')
            {
                depth++;
                continue;
            }
            if (c == '}')
            {
                depth--;
                continue;
            }
            if (depth != 1)
            {
                continue;
            }
            if ((c == 't' || c == 'T') && isNextStr("rack"))
            {
                trackFound = true;
                continue;
            }
            if ((c != 'n' && c != 'N') || !isNextStr("163ch"))
            {
                continue;
            }

            if (trackFound)
            {
                std::cerr << "Line " << linenum << " : N163CH must be specified before the first Track in Music." << std::endl;
                exit(1);
            }
            if (n163ChannelCountSpecified)
            {
                std::cerr << "Line " << linenum << " : N163CH is already specified in Music " << music << "." << std::endl;
                exit(1);
            }

            int count;
            skipSpace();
            if (!getMultiDigit(count) || count < 1 || count > 8)
            {
                std::cerr << "Line " << linenum << " : N163CH must be 1 to 8." << std::endl;
                exit(1);
            }

            n163ChannelCounts[music] = count;
            n163ChannelCountSpecified = true;
        }
    }

    for (const auto& [music, count] : n163ChannelCounts)
    {
        if (count > n163MaxChannelCount)
        {
            n163MaxChannelCount = count;
        }
    }
    n163WaveDataSize = 128 - n163MaxChannelCount * 8;
}


void MMLReader::readDifinitions()
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;
    bool isSub = false;

    while (ss.get(c))
    {
        skipSpace();
        skipComment();
        skipSpace();
        if (c == '#')
        {
            if (isNextStr("timebase"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    timebase = n;
                    lengthtbl.clear();
                    makeLengthTbl(lengthtbl);
                }
            }
            else if (isMusic || isTrack)
            {
                std::cerr << "Line " << linenum << " : Please write settings starting with '#' at the beginning of the file." << std::endl;
                exit(1);
            }
            else if (isNextStr("offsetpcm"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    dpcmoffset = n - 0xc000;
                }
            }
            else if (isNextStr("offsetv"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    v_offset = n;
                }
            }
            else if (isNextStr("offsetf"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    f_offset = n;
                }
            }
            else if (isNextStr("offsetn"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    n_offset = n;
                }
            }
            else if (isNextStr("offsett"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    t_offset = n;
                }
            }
            else if (isNextStr("title"))
            {
                getStrInQuote(title);
            }
            else if (isNextStr("artist"))
            {
                getStrInQuote(artist);
            }
            else if (isNextStr("copyright"))
            {
                getStrInQuote(copyright);
            }
            else if (isNextStr("expdevice"))
            {
                std::string str;
                while (!ss.eof())
                {
                    getc(c);
                    if (c == '\n')
                    {
                        break;
                    }
                    else
                    {
                        str += tolower(c);
                    }
                }

                if (str.find("vrc6") != std::string::npos)
                {
                    expdevice |= Expdev::VRC6;
                }

                if (str.find("vrc7") != std::string::npos)
                {
                    expdevice |= Expdev::VRC7;
                }

                if (str.find("fds") != std::string::npos)
                {
                    expdevice |= Expdev::FDS;
                }

                if (str.find("mmc5") != std::string::npos)
                {
                    expdevice |= Expdev::MMC5;
                }

                if (str.find("n163") != std::string::npos)
                {
                    expdevice |= Expdev::N163;
                }

                if (str.find("ss5b") != std::string::npos)
                {
                    expdevice |= Expdev::SS5B;
                }
            }
        }
        else if (c == '@')
        {
            if (isNextStr("dpcm"))   //DPCMリスト
            {
                skipSpace();
                if (isNextChar('{'))
                {
                    while (!ss.eof())
                    {
                        skipSpace();
                        skipComment();
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            int dpcmnum = n;
                            skipSpace();
                            if (getMultiDigit(n))
                            {
                                int dpcminit = n;
                                skipSpace();
                                if (isNextChar('"'))
                                {
                                    std::wstring str;
                                    while (ss.get(c))
                                    {
                                        if (c != '"')
                                        {
                                            str += c;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                    if (!str.empty())
                                    {
                                        if (!Utils::GetFullPath(str))
                                        {
                                            std::cerr << "Line " << linenum << " : Failed to get DPCM path." << std::endl;
                                            exit(1);
                                        }
                                    }
                                    else
                                    {
                                        std::cerr << "Line " << linenum << " : Missing DPCM path." << std::endl;
                                        exit(1);
                                    }
                                    DpcmInfo d;
                                    d.path = str;
									d.init = dpcminit;
                                    dpcmlist[dpcmnum] = d;
                                    skipSpace();
                                    skipComment();
                                    skipSpace();
                                    if (isNextChar('}'))
                                    {
                                        int offset = dpcmoffset;

                                        for (auto& [num, dpcm] : dpcmlist)
                                        {
                                            int size = (int)Utils::GetFileSize(dpcm.path);
                                            if (size > 0)
                                            {
                                                dpcm.offset = offset;
                                                dpcm.size = size;
                                                offset += size;
                                            }
                                            else
                                            {
                                                std::cerr << "Line " << linenum << " : Failed to get DPCM file size." << std::endl;
                                                exit(1);
                                            }
                                        }
                                        break;
                                    }
                                }
                                else
                                {
                                    std::cerr << "Line " << linenum << " : Missing \"." << std::endl;
                                    exit(1);
                                }
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : Missing DPCM Initial Value." << std::endl;
								exit(1);
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : Missing DPCM Number." << std::endl;
                            exit(1);
                        }
                    }
                }
                else
                {
                    std::cerr << "Line " << linenum << " : Missing {." << std::endl;
                    exit(1);
                }
            }
            else if (isNextChar('m'))   //マップ定義
            {
                if (!isTrack && !isSub)  //外にあったら定義
                {
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        if (mapdiflist.count(n) > 0)
                        {
                            std::cerr << "Line " << linenum << " : [Map difinition] Map diffinition #" << n << " is already exists." << std::endl;
                            exit(1);
                        }

                        NoteMapDif mapdif;
                        mapdif.number = n;

                        skipSpace();
                        if (isNextChar('{'))
                        {
                            skipSpace();
                            skipComment();
                            skipSpace();

                            //ここからノートナンバーと定義リストの列挙
                            while (!ss.eof())
                            {
                                NoteMap map;
                                skipSpaceUntilNextLine();
                                skipComment();
                                skipSpaceUntilNextLine();
                                getNoteNumber(map.target);

                                //コマンドの列挙
                                while (!ss.eof())
                                {
                                    int convert = 0;
                                    Command cmd;
                                    CommandArgs args;
                                    skipSpaceUntilNextLine();
                                    skipComment();
                                    skipSpaceUntilNextLine();

                                    if (getNoteNumber(convert))   //変換先ノートナンバー
                                    {
                                        map.convert = convert;
                                        cmd = map.target;
                                    }
                                    else if (getc(c))
                                    {
                                        if (c == '@')   //エンベロープか音色かデータ再生
                                        {
                                            skipSpaceUntilNextLine();
                                            getc(c);
                                            switch (c)
                                            {
                                            case 'v':
                                            case 'V':
                                                if (isNextChar('*'))
                                                {
                                                    cmd = VOLUME_ENV_STOP;
                                                }
                                                else
                                                {
                                                    cmd = VOLUME_ENV;
                                                    getCmdArgs(args);
                                                }
                                                break;
                                            case 'f':
                                            case 'F':
                                                if (isNextChar('*'))
                                                {
                                                    cmd = FREQ_ENV_STOP;
                                                }
                                                else
                                                {
                                                    cmd = FREQ_ENV;
                                                    getCmdArgs(args);
                                                }
                                                break;
                                            case 'n':
                                            case 'N':
                                                if (isNextChar('*'))
                                                {
                                                    cmd = NOTE_ENV_STOP;
                                                }
                                                else
                                                {
                                                    cmd = NOTE_ENV;
                                                    getCmdArgs(args);
                                                }
                                                break;
                                            case 't':
                                            case 'T':
                                                if (isNextChar('*'))
                                                {
                                                    cmd = TONE_ENV_STOP;
                                                }
                                                else
                                                {
                                                    cmd = TONE_ENV;
                                                    getCmdArgs(args);
                                                }
                                                break;
                                            case 'p':
                                            case 'P':
                                                cmd = PLAY_DATA;
                                                getCmdArgs(args);
                                                break;
                                            case '0':
                                            case '1':
                                            case '2':
                                            case '3':
                                            case '4':
                                            case '5':
                                            case '6':
                                            case '7':
                                            case '8':
                                            case '9':
                                                cmd = TONE;
                                                ss.seekg((int)ss.tellg() - 1);
                                                getMultiDigit(n);
												args.push_back(n);
                                                break;
                                            default:
                                                std::cerr << "Line " << linenum << " : [Map difinition] Invalid command." << std::endl;
                                                exit(1);
                                            }
                                        }
                                        else if (c == 's' || c == 'S')  //ソフトウェアスイープ
                                        {
                                            cmd = SW_SWEEP;
                                            getCmdArgs(args);
                                        }
                                        else if (c == 'q')  //ゲートタイムq
                                        {
                                            cmd = GATE_TIME;
                                            getCmdArgs(args);
                                            if (args[0] > 0)
                                            {
                                                args[0] |= 1 << 6;
                                            }
                                        }
                                        else if (c == 'u')  //ゲートタイムu
                                        {
                                            cmd = GATE_TIME;
                                            getCmdArgs(args);
                                            if (args[0] > 0)
                                            {
                                                args[0] |= 2 << 6;
                                            }
                                        }
                                        else if (c == 'Q')  //ゲートタイムQ
                                        {
                                            cmd = GATE_TIME;
                                            getCmdArgs(args);
                                            if (args[0] > 0)
                                            {
                                                args[0] |= 3 << 6;
                                            }
                                        }
                                        else if (c == 'K')  //キーシフト絶対指定
                                        {
                                            cmd = KEYSHIFT_ABS;
                                            getCmdArgs(args);
                                        }
                                        else if (c == 'v' || c == 'V')  //ボリューム
                                        {
                                            cmd = VOLUME;
                                            getCmdArgs(args);
                                        }
                                        else if (c == 'r' || c == 'R')  //休符
                                        {
                                            cmd = REST_DEFLEN;
                                        }
                                        else if (c == 'w' || c == 'W')  //メモリ書き込み
                                        {
                                            cmd = MEM_WRITE;
                                            int n;
                                            skipSpaceUntilNextLine();
                                            if (getMultiDigit(n))     //コマンド内容を保存
                                            {
                                                args.push_back(0xff & n);
                                                args.push_back((n >> 8) & 0xff);
                                                skipSpaceUntilNextLine();
                                                if (isNextChar(','))
                                                {
                                                    if (getMultiDigit(n))
                                                    {
                                                        args.push_back(n);
                                                    }
                                                }
                                            }
                                        }
                                        else if (c == '\n')    //改行で終了
                                        {
                                            linenum++;
                                            break;
                                        }
                                        else if (c == '}')   //閉じかっこが来たら定義終了
                                        {
                                            break;
                                        }
                                        else
                                        {
                                            std::cerr << "Line " << linenum << " : [Map difinition] Invalid command." << std::endl;
                                            exit(1);
                                        }
                                    }
                                    map.commands[cmd] = args;
                                }

                                mapdif.maplist[map.target] = map;

                                skipSpace();
                                skipComment();
                                skipSpace();

                                if (c == '}' || isNextChar('}'))   //閉じかっこが来たら定義終了
                                {
                                    mapdiflist[mapdif.number] = mapdif;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == '\\')
        {
            isSub = true;
        }
        else if (c == '}')
        {
            if (isSub)
            {
                isSub = false;
            }
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }
}


void MMLReader::readMacro(std::map<std::string, std::string, cmpByStringLength>& macrolist)
{
    char c;
    bool isTrack = false;
    bool isMusic = false;

    while (ss.get(c))
    {
        skipSpace();
        skipComment();
        skipSpace();
        if (c == '$')
        {
            getc(c);
            if (c != '$')
            {
                if (!isMusic)
                {
                    std::string mname;
                    std::string mbody;

                    while (c != '{' && !ss.eof())
                    {
                        mname += c;
                        skipSpace();
                        skipComment();
                        skipSpace();
                        getc(c);
                    }

                    if (!mname.empty())
                    {
                        if (macrolist.count(mname))
                        {
                            std::cerr << "Line " << linenum << " : Macro $" << mname << " is already exists." << std::endl;
                            exit(1);
                        }
                    }

                    getc(c);
                    while (c != '}' && !ss.eof())
                    {
                        mbody += c;
                        skipSpace();
                        skipComment();
                        skipSpace();
                        getc(c);
                    }

                    if (!mbody.empty())
                    {
                        macrolist[mname] = mbody;
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == '}')
        {
            if (isMusic)
            {
                isMusic = false;
                isTrack = false;
            }
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }
}


void MMLReader::replaceMacro(std::map<std::string, std::string, cmpByStringLength>& macrolist, std::stringstream& ss2)
{
    bool isMusic = false;
    bool isSub = false;
    std::string buf;

    while (std::getline(ss, buf))
    {
        std::string lower;
        std::transform(buf.begin(), buf.end(), std::back_inserter(lower), tolower);

        if (lower.find("music") != std::string::npos)
        {
            isMusic = true;
        }
        else if (!isMusic && buf.find("\\") != std::string::npos)    //サブルーチン定義
        {
            isSub = true;
        }

        if (isMusic || isSub)
        {
            auto bpos = buf.find('}');

            if (isMusic && bpos != std::string::npos)
            {
                isMusic = false;
            }
            else if (isSub && bpos != std::string::npos)
            {
                isSub = false;
            }

            for (const auto& [k, v] : macrolist)
            {
                std::string name = "$" + k;

                while (true && !ss.eof())
                {
                    auto rpos = buf.find(name);
                    if (rpos != std::string::npos && rpos < bpos)
                    {
                        std::string pre = buf.substr(0, rpos);
                        std::string post = buf.substr(rpos + name.length(), buf.length());
                        buf = pre + v + post;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        ss2 << buf << std::endl;
        linenum++;
    }
}


void MMLReader::readSubRoutine(int& subsize)
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;

    auto insertN163WaveSetup = [&](std::vector<unsigned char>& bytes)
    {
        if (!(expdevice & Expdev::N163))
        {
            return;
        }

        std::vector<unsigned char> patched;
        patched.reserve(bytes.size());

        int currentSetup = -1;
        for (size_t i = 0; i < bytes.size();)
        {
            if (bytes[i] == N163_WAVE_SETUP && i + 4 < bytes.size())
            {
                currentSetup = bytes[i + 1] | (bytes[i + 2] << 8) | (bytes[i + 3] << 16);
                patched.insert(patched.end(), bytes.begin() + i, bytes.begin() + i + 5);
                i += 5;
                continue;
            }

            if (bytes[i] == TONE && i + 1 < bytes.size())
            {
                int tone = bytes[i + 1];
                if (tone >= 0 && tone < static_cast<int>(n163WaveOffsets.size()))
                {
                    int offset = n163WaveOffsets[tone];

                    int setup = offset | (n163WaveLengthRegs[tone] << 8) | (n163WaveLengthUnits[tone] << 16);
                    if (setup != currentSetup)
                    {
                        patched.push_back(N163_WAVE_SETUP);
                        patched.push_back(static_cast<unsigned char>(offset));
                        patched.push_back(n163WaveLengthRegs[tone]);
                        int tableOffset = n163FreqTableOffsets.at(n163WaveLengthUnits[tone]);
                        patched.push_back(static_cast<unsigned char>(tableOffset));
                        patched.push_back(static_cast<unsigned char>(tableOffset >> 8));
                        currentSetup = setup;
                    }
                }
            }

            patched.push_back(bytes[i]);
            i++;
        }

        bytes.swap(patched);
    };

    auto applyTimebaseAt = [&](int endpos)
    {
        std::string src = ss.str();
        if (endpos < 0 || endpos > static_cast<int>(src.size()))
        {
            endpos = static_cast<int>(src.size());
        }

        int activeTimebase = 96;
        size_t lineStart = 0;
        while (lineStart < static_cast<size_t>(endpos))
        {
            size_t lineEnd = src.find('\n', lineStart);
            if (lineEnd == std::string::npos)
            {
                lineEnd = src.size();
            }

            size_t endLimit = static_cast<size_t>(endpos);
            size_t scanEnd = lineEnd < endLimit ? lineEnd : endLimit;
            std::string line = src.substr(lineStart, scanEnd - lineStart);
            size_t comment = line.find("//");
            if (comment != std::string::npos)
            {
                line.resize(comment);
            }

            size_t pos = 0;
            while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\r'))
            {
                pos++;
            }

            if (pos < line.size() && line[pos] == '#')
            {
                pos++;
                while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
                {
                    pos++;
                }

                const std::string key = "timebase";
                bool match = pos + key.size() <= line.size();
                for (size_t j = 0; match && j < key.size(); j++)
                {
                    match = std::tolower(static_cast<unsigned char>(line[pos + j])) == key[j];
                }

                if (match)
                {
                    pos += key.size();
                    while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t'))
                    {
                        pos++;
                    }

                    std::string value;
                    while (pos < line.size() && line[pos] >= '0' && line[pos] <= '9')
                    {
                        value += line[pos];
                        pos++;
                    }

                    if (!value.empty())
                    {
                        activeTimebase = std::atoi(value.c_str());
                    }
                }
            }

            if (lineEnd == src.size())
            {
                break;
            }
            lineStart = lineEnd + 1;
        }

        timebase = activeTimebase;
        lengthtbl.clear();
        makeLengthTbl(lengthtbl);
    };


    while (ss.get(c))
    {
        skipSpace();
        skipComment();

        if (c == '\\')
        {
            if (!isTrack && !isMusic)
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    if (subdata.count(n))   //多重定義防止
                    {
                        std::cerr << "Line " << linenum << " : Subroutine #" << n << " is already exists." << std::endl;
                        exit(1);
                    }

                    skipSpace();
                    if (isNextChar('{'))
                    {
                        int pos = (int)ss.tellg();
                        applyTimebaseAt(pos);
                        std::vector<unsigned char> trhead;
                        std::vector<unsigned char> trbody;
                        int tone = 0;
                        readBrackets(pos, 0, trhead, trbody, tone);
                        insertN163WaveSetup(trbody);
                        SubData sd;
                        sd.num = n;
                        sd.addr = totalpos + subsize;
                        sd.tone = tone;
                        trbody.push_back(LOOP_MID_END);    //ループ途中終了コードで戻る
                        sd.data = trbody;
                        subdata[n] = sd;
                        subsize += trbody.size();
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == '}')
        {
            if (isMusic)
            {
                isMusic = false;
                isTrack = false;
            }
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }
}


void MMLReader::readEnvelope(int& envsize)
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;

    while (ss.get(c))
    {
        skipSpace();
        skipComment();

        if (c == '@')   //音色指定か各種定義
        {
            if (!isTrack && !isMusic)
            {
                skipSpace();
                if (isNextChar('e'))
                {
                    if (isTrack)
                    {
                        std::cerr << "Line " << linenum << " : Do not write envelope difinition in the track." << std::endl;
                        exit(1);
                    }

                    if (isMusic)
                    {
                        std::cerr << "Line " << linenum << " : Do not write envelope difinition in the music." << std::endl;
                        exit(1);
                    }
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        int envnum = n;
                        if (envdata.count(n))   //多重定義防止
                        {
                            std::cerr << "Line " << linenum << " : [Envelope difinition] Envelope #" << envnum << " is already exists." << std::endl;
                            exit(1);
                        }

                        skipSpace();
                        if (isNextChar('{'))
                        {
                            EnvData env;
                            std::vector<unsigned char> ehead;
                            std::vector<unsigned char> ebody;

                            while (!ss.eof())
                            {
                                skipSpace();
                                if (getMultiDigit(n))
                                {
                                    ebody.push_back(n);
                                    ebody.push_back(1); //省略されたら1フレーム
                                }
                                else if (getc(c))
                                {
                                    if (c == 'f' || c == 'F')
                                    {
                                        if (getMultiDigit(n) && n >= 0)
                                        {
                                            ebody[ebody.size() - 1] = n;
                                        }
                                    }
                                    else if (c == '|')
                                    {
                                        ehead.push_back((int)(ebody.size()) / 2 + 1);  //ヘッダ自身を含めると+1

                                        if (ehead.size() > 2)
                                        {
                                            std::cerr << "Line " << linenum << " : [Envelope difinition] Too many '|'." << std::endl;
                                            exit(1);
                                        }
                                    }
                                    else if (c == '}')
                                    {

                                        if (ehead.size() == 0)
                                        {
                                            ehead.push_back(0x81);
                                            ehead.push_back((int)(ebody.size()) / 2 + 1);
                                            ebody.push_back(0); //最後に0を追加にして次のエンベロープに流れないようにする
                                            ebody.push_back(0);
                                        }
                                        else if (ehead.size() == 1)
                                        {
                                            //ヘッダ1個だったらリリースが省略されたとみなす
                                            ehead[0] |= 0x80;
                                            ehead.push_back((int)(ebody.size()) / 2 + 1);
                                            ebody.push_back(0); //最後に0を追加にして次のエンベロープに流れないようにする
                                            ebody.push_back(0);
                                        }
                                        else
                                        {
                                            //閉じカッコが来たら保存して抜ける
                                            ebody[ebody.size() - 1] = 0; //最後の値を0にして次のエンベロープに流れないようにする
                                        }

                                        std::copy(ehead.begin(), ehead.end(), std::back_inserter(env.data));
                                        std::copy(ebody.begin(), ebody.end(), std::back_inserter(env.data));
                                        env.num = envnum;
                                        env.addr = totalpos + envsize;
                                        envsize += env.data.size();
                                        envdata[envnum] = env;
                                        break;
                                    }
                                    else
                                    {
                                        std::cerr << "Line " << linenum << " : [Envelope difinition] Invalid character." << std::endl;
                                        exit(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : [Envelope difinition] Missing {." << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : [Envelope difinition] No envelope number." << std::endl;
                        exit(1);
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }
}


void MMLReader::readVrc7Patch()
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;
    vrc7Patches.clear();

    while (ss.get(c))
    {
        skipSpace();
        skipComment();
        if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }

        if (isTrack || isMusic || c != '@' || !isNextStr("vrc7u"))
        {
            if (c == '\n')
            {
                linenum++;
            }
            continue;
        }

        skipSpace();
        if (!getMultiDigit(n) || n < 0 || n > 255)
        {
            std::cerr << "Line " << linenum << " : [VRC7 patch definition] Patch number must be 0 to 255." << std::endl;
            exit(1);
        }
        int patchnum = n;

        skipSpace();
        if (!isNextChar('{'))
        {
            continue;   //マクロ本体などにある @vrc7u番号 は選択コマンド
        }
        if (vrc7Patches.count(patchnum))
        {
            std::cerr << "Line " << linenum << " : [VRC7 patch definition] Patch #" << patchnum << " is already defined." << std::endl;
            exit(1);
        }

        std::vector<unsigned char> patch;

        while (!ss.eof())
        {
            skipSpace();
            if (getMultiDigit(n))
            {
                if (n < 0 || n > 255 || patch.size() >= 8)
                {
                    std::cerr << "Line " << linenum << " : [VRC7 patch definition] Patch data must be exactly 8 bytes (0 to 255)." << std::endl;
                    exit(1);
                }
                patch.push_back(static_cast<unsigned char>(n));
            }
            else if (getc(c))
            {
                if (c == '}')
                {
                    break;
                }
                std::cerr << "Line " << linenum << " : [VRC7 patch definition] Invalid character." << std::endl;
                exit(1);
            }
        }

        if (patch.size() != 8)
        {
            std::cerr << "Line " << linenum << " : [VRC7 patch definition] Patch data must be exactly 8 bytes." << std::endl;
            exit(1);
        }
        vrc7Patches[patchnum] = patch;
    }
}


void MMLReader::readWaveData(std::vector<unsigned char>& out)
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;
    out.clear();
    std::map<int, std::vector<unsigned char>> wavdata;

    while (ss.get(c))
    {
	    bool fdsw = false;
        bool n163w = false;
        skipSpace();
        skipComment();

        if (c == '@')   //音色指定か各種定義
        {
            if (!isTrack && !isMusic)
            {
                skipSpace();
                if (isNextStr("fdsw"))
                {
                    fdsw = true;
                }
                else if (isNextStr("n163w"))
                {
                    n163w = true;
                }

                if (fdsw || n163w)
                {
                    if (isTrack)
                    {
                        std::cerr << "Line " << linenum << " : Do not write wave difinition in the track." << std::endl;
                        exit(1);
                    }

                    if (isMusic)
                    {
                        std::cerr << "Line " << linenum << " : Do not write wave difinition in the music." << std::endl;
                        exit(1);
                    }
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        int wavnum = n;
                        if (wavdata.count(n))   //多重定義防止
                        {
                            std::cerr << "Line " << linenum << " : [Wave difinition] Wave #" << wavnum << " is already exists." << std::endl;
                            exit(1);
                        }

                        skipSpace();
                        if (isNextChar('{'))
                        {
                            while (!ss.eof())
                            {
                                skipSpace();
                                if (getMultiDigit(n))
                                {
                                    if (fdsw)
                                    {
                                        if (n < 0 || n > 63)
                                        {
                                            std::cerr << "Line " << linenum << " : [Wave difinition] Waveform data must be 0 to 63." << std::endl;
                                            exit(1);
                                        }

                                        if (wavdata[wavnum].size() >= 64)
                                        {
                                            std::cerr << "Line " << linenum << " : [Wave difinition] Too many waveform data." << std::endl;
                                            exit(1);
                                        }
                                    }
                                    else if (n163w)
                                    {
                                        if (wavnum < 0 || wavnum >= 16)
                                        {
                                            std::cerr << "Line " << linenum << " : [Wave difinition] N163 wave number must be 0 to 15." << std::endl;
                                            exit(1);
                                        }

                                        if (n < 0 || n > 15)
                                        {
                                            std::cerr << "Line " << linenum << " : [Wave difinition] N163 waveform data must be 0 to 15." << std::endl;
                                            exit(1);
                                        }

                                        if (wavdata[wavnum].size() >= static_cast<size_t>(n163WaveDataSize * 2))
                                        {
                                            std::cerr << "Line " << linenum << " : [Wave difinition] Too many N163 waveform data." << std::endl;
                                            exit(1);
                                        }
                                    }

                                    wavdata[wavnum].push_back(n);
                                }
                                else if (getc(c))
                                {
                                    if (c == '}')
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        std::cerr << "Line " << linenum << " : [Wave difinition] Invalid character." << std::endl;
                                        exit(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : [Wave difinition] Missing {." << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : [Wave difinition] No wave number." << std::endl;
                        exit(1);
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }

    if (expdevice & Expdev::N163)
    {
        out.assign(n163WaveDataSize, 0x88);
        n163WaveOffsets.assign(16, 0);
        n163WaveLengthRegs.assign(16, 0xe0);
        n163WaveLengthUnits.assign(16, 8);

        int sampleOffset = 0;
        for (const auto& [wavnum, data] : wavdata)
        {
            int length = static_cast<int>(data.size());
            int maxSamples = n163WaveDataSize * 2;
            if (length < 4 || length > maxSamples || length % 4 != 0)
            {
                std::cerr << "Line " << linenum << " : [Wave difinition] N163 wave length must be a multiple of 4 from 4 to " << maxSamples << "." << std::endl;
                exit(1);
            }

            if (sampleOffset + length > maxSamples)
            {
                std::cerr << "Line " << linenum << " : [Wave difinition] Too much N163 waveform data. Total wave length must be " << maxSamples << " samples or less." << std::endl;
                exit(1);
            }

            n163WaveOffsets[wavnum] = static_cast<unsigned char>(sampleOffset);
            n163WaveLengthRegs[wavnum] = static_cast<unsigned char>(256 - length);
            n163WaveLengthUnits[wavnum] = static_cast<unsigned char>(length / 4);

            for (int i = 0; i < length; i += 2)
            {
                unsigned char lo = data[i];
                unsigned char hi = (i + 1 < length) ? data[i + 1] : 8;
                out[(sampleOffset + i) / 2] = lo | (hi << 4);
            }

            sampleOffset += length;
        }
    }
    else
    {
        for (const auto& [wavnum, data] : wavdata)
        {
            std::copy(data.begin(), data.end(), std::back_inserter(out));
        }
    }
}

void MMLReader::readModData(std::vector<unsigned char>& out)
{
    char c;
    int n;
    bool isTrack = false;
    bool isMusic = false;
    out.clear();
    std::map<int, std::vector<unsigned char>> moddata;

    while (ss.get(c))
    {
        bool fdsm = false;
        skipSpace();
        skipComment();

        if (c == '@')   //音色指定か各種定義
        {
            if (!isTrack && !isMusic)
            {
                skipSpace();
                if (isNextStr("fdsm"))
                {
                    fdsm = true;
                }

                if (fdsm)
                {
                    if (isTrack)
                    {
                        std::cerr << "Line " << linenum << " : Do not write modulation difinition in the track." << std::endl;
                        exit(1);
                    }

                    if (isMusic)
                    {
                        std::cerr << "Line " << linenum << " : Do not write modulation difinition in the music." << std::endl;
                        exit(1);
                    }
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        int wavnum = n;
                        if (moddata.count(n))   //多重定義防止
                        {
                            std::cerr << "Line " << linenum << " : [Modulation difinition] Wave #" << wavnum << " is already exists." << std::endl;
                            exit(1);
                        }

                        skipSpace();
                        if (isNextChar('{'))
                        {
                            while (!ss.eof())
                            {
                                skipSpace();
                                if (getMultiDigit(n))
                                {
                                    if (fdsm)
                                    {
                                        if (n < 0 || n > 8)
                                        {
                                            std::cerr << "Line " << linenum << " : [Modulation difinition] Modulation waveform data must be 0 to 8." << std::endl;
                                            exit(1);
                                        }

                                        if (moddata[wavnum].size() >= 32)
                                        {
                                            std::cerr << "Line " << linenum << " : [Modulation difinition] Too many modulation waveform data." << std::endl;
                                            exit(1);
                                        }
                                    }

                                    moddata[wavnum].push_back(n);
                                }
                                else if (getc(c))
                                {
                                    if (c == '}')
                                    {
                                        break;
                                    }
                                    else
                                    {
                                        std::cerr << "Line " << linenum << " : [Modulation difinition] Invalid character." << std::endl;
                                        exit(1);
                                    }
                                }
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : [Modulation difinition] Missing {." << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : [Modulation difinition] No wave number." << std::endl;
                        exit(1);
                    }
                }
            }
        }
        else if (c == '\n')
        {
            linenum++;
        }
        else if (c == 't' || c == 'T')
        {
            if (isNextStr("rack"))
            {
                isTrack = true;
            }
        }
        else if (c == 'm' || c == 'M')
        {
            if (isNextStr("usic"))
            {
                isMusic = true;
            }
        }
    }


    for (const auto& [wavnum, data] : moddata)
    {
        int temp = 0;
        for (int i = 0, size = (int)data.size(); i < size; i++)
        {
            if (i % 2 == 0)   //2バイトごとに1バイトにまとめる
            {
                temp = data[i];
            }
            else
            {
                temp |= (data[i] << 4);
                out.push_back(temp);
            }
		}
    }
}


void MMLReader::readBrackets(int startpos, int trheadsize, std::vector<unsigned char>& trhead, std::vector<unsigned char>& trbody, int& tone, int n163ChannelCount)
{
    struct DefLenGuard
    {
        int& target;
        int value;

        ~DefLenGuard()
        {
            target = value;
        }
    } defLenGuard{ deflen, deflen };

    char c;
    int n;
    int volume = 15;
    int octave = 4;
    int grace = 0;
    long long lengthError = 0;
    long long lengthErrorScale = 192;
    int loopmid_volume = 15;
	int loopmid_octave = 4;
    int loopmid_tone = 0;
    bool isTrack = false;
    bool isMusic = false;
    bool usePDelay = false;
    bool isLooped = false;
	bool isLoopedMid = false;
    bool dpcmToneSpecified = false;
    bool dpcmDefaultRequired = false;
    bool fdsToneSpecified = false;
    bool fdsModSpecified = false;
    int sweepStart = 0;
    int normalizedSweepStops = 0;
    bool sweepStopBeforeSlur = false;
    int pddist = 4;
    int pdvol = 0;
    int pdlen = 0;
    int shorten = 0;
    bool deflenIsFrames = false;
    int usingNoteMap = -1;
    std::map<Command, CommandArgs> usingCmds;
    std::vector<TrackData> tracks;
    TrackData tr;
    std::queue<unsigned char> prevnotes;
	std::vector<unsigned char> data;

    auto skipMmlTrivia = [](const std::string& source, size_t& pos)
    {
        while (pos < source.size())
        {
            char current = source[pos];
            if (current == ' ' || current == '\t' || current == '\r' || current == '\n')
            {
                pos++;
            }
            else if (current == '/' && pos + 1 < source.size() && source[pos + 1] == '/')
            {
                pos += 2;
                while (pos < source.size() && source[pos] != '\n')
                {
                    pos++;
                }
            }
            else if (current == '/' && pos + 1 < source.size() && source[pos + 1] == '*')
            {
                pos += 2;
                while (pos + 1 < source.size() && !(source[pos] == '*' && source[pos + 1] == '/'))
                {
                    pos++;
                }
                if (pos + 1 < source.size())
                {
                    pos += 2;
                }
            }
            else
            {
                break;
            }
        }
    };

    auto isSweepStopAt = [&](const std::string& source, size_t pos)
    {
        skipMmlTrivia(source, pos);
        if (pos >= source.size() || (source[pos] != 's' && source[pos] != 'S'))
        {
            return false;
        }

        pos++;
        skipMmlTrivia(source, pos);
        return pos < source.size() && source[pos] == '*';
    };

    auto nextTokenIsSlur = [&]()
    {
        std::streampos streampos = ss.tellg();
        if (streampos == std::streampos(-1))
        {
            return false;
        }

        std::string source = ss.str();
        size_t pos = static_cast<size_t>(streampos);
        skipMmlTrivia(source, pos);
        return pos < source.size() && source[pos] == '&';
    };

    auto slurTargetHasSweepStop = [&]()
    {
        std::streampos streampos = ss.tellg();
        if (streampos == std::streampos(-1))
        {
            return false;
        }

        std::string source = ss.str();
        size_t pos = static_cast<size_t>(streampos);
        if (isSweepStopAt(source, pos))
        {
            return true;
        }

        skipMmlTrivia(source, pos);
        if (pos >= source.size() || !((source[pos] >= 'a' && source[pos] <= 'g') ||
            (source[pos] >= 'A' && source[pos] <= 'G')))
        {
            return false;
        }

        pos++;
        skipMmlTrivia(source, pos);
        while (pos < source.size() && (source[pos] == '+' || source[pos] == '-'))
        {
            pos++;
            skipMmlTrivia(source, pos);
        }

        while (pos < source.size())
        {
            skipMmlTrivia(source, pos);
            if (pos >= source.size())
            {
                break;
            }

            char current = source[pos];
            if ((current >= '0' && current <= '9') || current == '.' || current == '%' ||
                current == '~' || current == '^')
            {
                pos++;
                continue;
            }
            break;
        }

        return isSweepStopAt(source, pos);
    };

    bool preserveDefaultRest = trheadsize == 0;
    deflen = 4;

    auto setDpcmDefault = [&]()
    {
        if (tr.device != DEV_2A03_DPCM || !dpcmDefaultRequired)
        {
            return;
        }

        if (!dpcmlist.count(0))
        {
            std::cerr << "Line " << linenum << " : DPCM #0 is not registered. Specify a DPCM tone before the first note." << std::endl;
            exit(1);
        }

        const auto& dpcm = dpcmlist.at(0);
        std::vector<unsigned char> defaults
        {
            TONE,
            static_cast<unsigned char>(dpcm.offset / 0x40),
            static_cast<unsigned char>(dpcm.size / 0x10),
            static_cast<unsigned char>(dpcm.init)
        };
        data.insert(data.begin(), defaults.begin(), defaults.end());
    };

    auto setFdsDefaults = [&]()
    {
        if (tr.device != DEV_FDS)
        {
            return;
        }

        std::vector<unsigned char> defaults;
        if (!fdsToneSpecified)
        {
            defaults.push_back(TONE);
            defaults.push_back(0);
        }
        if (!fdsModSpecified)
        {
            defaults.push_back(FDS_MOD_TONE);
            defaults.push_back(0);
        }

        if (defaults.empty())
        {
            return;
        }

        auto insertpos = data.begin();
        if (data.size() >= 2 && data[0] == HW_ENV && data[1] == 0x80)
        {
            insertpos += 2;
        }
        data.insert(insertpos, defaults.begin(), defaults.end());
    };

    int n163WaveSetup = -1;
    auto pushN163WaveSetup = [&](int tone)
    {
        if (!(expdevice & Expdev::N163) || tr.device < DEV_N163_CH1 || tr.device > DEV_N163_CH8)
        {
            return;
        }

        if (tone < 0 || tone >= static_cast<int>(n163WaveOffsets.size()))
        {
            std::cerr << "Line " << linenum << " : N163 wave number must be 0 to 15." << std::endl;
            exit(1);
        }

        unsigned char offset = n163WaveOffsets[tone];

        int setup = offset | (n163WaveLengthRegs[tone] << 8) | (n163WaveLengthUnits[tone] << 16);
        if (setup == n163WaveSetup)
        {
            return;
        }

        data.push_back(N163_WAVE_SETUP);
        data.push_back(offset);
        data.push_back(n163WaveLengthRegs[tone]);
        int tableOffset = n163FreqTableOffsets.at(n163WaveLengthUnits[tone]);
        data.push_back(static_cast<unsigned char>(tableOffset));
        data.push_back(static_cast<unsigned char>(tableOffset >> 8));
        n163WaveSetup = setup;
    };

    auto pushVrc7Patch = [&](int patchnum)
    {
        if (tr.device < DEV_VRC7_CH1 || tr.device > DEV_VRC7_CH6)
        {
            return;
        }
        if (!vrc7Patches.count(patchnum))
        {
            std::cerr << "Line " << linenum << " : VRC7 custom instrument #" << patchnum << " is not defined." << std::endl;
            exit(1);
        }
        data.push_back(VRC7_PATCH);
        const auto& patch = vrc7Patches[patchnum];
        data.insert(data.end(), patch.begin(), patch.end());
    };

    ss.seekg(startpos);
    int note = 0;

    while (ss.get(c))
    {
        int nn = 0;        //コマンドの値
        int num = 0;        //コマンドに渡す数値等
        int tmp = 0;
        std::string digit;

        switch (c)
        {
        case ' ':
        case '\t':
        case '\r':
            break;
        case '\n':
            linenum++;
            break;
        case '/':
            getc(c);
            if (c == '/')      //1行コメント
            {
                if (!skipUntil("\n"))   //行末まで飛ばす
                {
                    std::cerr << "No end of comment." << std::endl;
                    exit(1);
                }
                linenum++;
            }
            else if (c == '*')          //複数行コメントの始まり
            {
                if (!skipUntil("*/"))   //コメントの終わりまで飛ばす
                {
                    std::cerr << "No end of comment." << std::endl;
                    exit(1);
                }
            }
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
            tmp = 0x20;
            [[fallthrough]];
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
            note = std::clamp(c - tmp - 0x61, 0, 7);
            nn = notetbl[note] + octave * 12;
            skipSpace();
            while (getc(c))          //半音
            {
                if (c == '+')
                {
                    nn += 1;
                }
                else if (c == '-')
                {
                    nn -= 1;
                }
                else
                {
                    ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
                    break;
                }
                skipSpace();
            }
            //ノートマップの処理。ノートマップ有効で定義と変換先が存在すれば実行
            if (usingNoteMap >= 0 && mapdiflist.count(usingNoteMap) && mapdiflist[usingNoteMap].maplist.count(nn))
            {
                int newNN = nn;
                NoteMap map = mapdiflist[usingNoteMap].maplist[nn];

                for (const auto& [cmd, args] : map.commands)
                {
                    if (!isLooped && usingCmds.count(cmd) > 0 && usingCmds[cmd] == args)    //コマンドが既に指定されている場合何もしない（ループ直後以外）
                    {
                        continue;
                    }

                    if (cmd == VOLUME_ENV || cmd == FREQ_ENV || cmd == NOTE_ENV || cmd == TONE_ENV)
                    {
                        int offset = 0;

                        switch (cmd)
                        {
                        case VOLUME_ENV:
                            offset = v_offset; break;
                        case FREQ_ENV:
                            offset = f_offset; break;
                        case NOTE_ENV:
                            offset = n_offset; break;
                        case TONE_ENV:
                            offset = t_offset; break;
                        }

                        if (args.size() >= 2)
                        {
                            auto& selectedEnvdata = (cmd == FREQ_ENV && tr.device >= DEV_VRC7_CH1 && tr.device <= DEV_VRC7_CH6)
                                ? vrc7FreqEnvdata : envdata;
                            pushEnvAssign(data, selectedEnvdata, cmd, args[0], args[1], true, offset);
                        }
                        else if (args.size() == 1)
                        {
                            auto& selectedEnvdata = (cmd == FREQ_ENV && tr.device >= DEV_VRC7_CH1 && tr.device <= DEV_VRC7_CH6)
                                ? vrc7FreqEnvdata : envdata;
                            pushEnvAssign(data, selectedEnvdata, cmd, args[0], 0, true, offset);
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : [Map] Missing args." << std::endl;
                            exit(1);
                        }

                        usingCmds[cmd] = args;
                        if (usingCmds.count(cmd + 1))
                        {
                            usingCmds.erase(cmd + 1);   //停止コマンドがあったら消す
                        }
                    }
                    else if (cmd == SW_SWEEP)
                    {
                        unsigned char arg1, arg2;
                        if (args.size() != 4 || !packSoftwareSweep(args[0], args[1], args[2], args[3], arg1, arg2))
                        {
                            std::cerr << "Line " << linenum << " : [Map] Software sweep argument is out of range." << std::endl;
                            exit(1);
                        }

                        data.push_back(cmd);
                        data.push_back(arg1);
                        data.push_back(arg2);
                        sweepStart = args[0];

                        usingCmds[cmd] = args;
                        if (usingCmds.count(cmd + 1))
                        {
                            usingCmds.erase(cmd + 1);   //停止コマンドがあったら消す
                        }
                    }
                    else if (cmd == PLAY_DATA)      //毎回実行するので使用中コマンドには追加しない
                    {
                        bool hasmusic = false;
                        for (int i = 0, size = (int)musiclist.size(); i < size; i++)
                        {
                            if (musiclist[i] == args[0])
                            {
                                data.push_back(PLAY_DATA);
                                data.push_back(musiclist[i]);
                                hasmusic = true;
                            }
                        }

                        if (!hasmusic)
                        {
                            std::cerr << "Line " << linenum << " : Music #" << args[0] << " is not registered." << std::endl;
                            exit(1);
                        }
                    }
                    else if (cmd == VOLUME_ENV_STOP || cmd == FREQ_ENV_STOP || cmd == NOTE_ENV_STOP || cmd == TONE_ENV_STOP || cmd == SW_SWEEP_STOP)
                    {
                        data.push_back(cmd);

                        for (int i = 0, size = (int)args.size(); i < size; i++)
                        {
                            data.push_back(args[i]);
                        }

                        usingCmds[cmd] = args;

                        if (usingCmds.count(cmd - 1))
                        {
                            usingCmds.erase(cmd - 1);   //開始コマンドがあったら消す
                        }
                    }
                    else if (cmd == TONE)
                    {
                        if (args[0] == 0)
                        {
                            pushVrc7Patch(0);
                        }
                        if (tr.device >= DEV_N163_CH1 && tr.device <= DEV_N163_CH8)
                        {
                            pushN163WaveSetup(args[0]);
                        }
                        data.push_back(TONE);

                        if (tr.device == DEV_2A03_DPCM)     //DPCMトラックなら
                        {
                            if (dpcmlist.count(args[0]))
                            {
                                data.push_back(dpcmlist[args[0]].offset / 0x40);
                                data.push_back(dpcmlist[args[0]].size / 0x10);
                                data.push_back(dpcmlist[args[0]].init);
                                dpcmToneSpecified = true;
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : [Map] DPCM #" << args[0] << " is not registered." << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            data.push_back(args[0]);
                            tone = args[0];
                            if (tr.device == DEV_FDS)
                            {
                                fdsToneSpecified = true;
                            }
                        }
                        usingCmds[cmd] = args;
                    }
                    else if (cmd > 0x6b && cmd != REST_DEFLEN)  //ノートと休符以外
                    {
                        data.push_back(cmd);

                        for (int i = 0, size = (int)args.size(); i < size; i++)
                        {
                            data.push_back(args[i]);
                        }
                        usingCmds[cmd] = args;
                    }
                }

                isLooped = false;

                if (map.convert >= 0)
                {
                    //ノートの変換先が存在した場合変換する
                    newNN = map.convert;
                }

                if (map.commands.count(REST_DEFLEN))   //休符コマンドが入っていた場合
                {
                    skipSpace();
                    calcLength(REST_DEFLEN, data, lengthtbl, grace, 0, lengthError, lengthErrorScale, deflenIsFrames, preserveDefaultRest);
                }
                else
                {
                    if (tr.device == DEV_2A03_NOISE || tr.device == DEV_2A03_DPCM)
                    {
                        if (tr.device == DEV_2A03_DPCM && !dpcmToneSpecified)
                        {
                            dpcmDefaultRequired = true;
                        }
                        //newNN = 0x0f - newNN & 0x0f;  //ドライバ側でやる
                        newNN = newNN & 0x0f;
                    }
                    else
                    {
                        newNN += sweepStart;
                    }

                    skipSpace();
                    calcLength(newNN, data, lengthtbl, grace, 0, lengthError, lengthErrorScale, deflenIsFrames);
                }
            }
            else
            {
                if (tr.device == DEV_2A03_NOISE || tr.device == DEV_2A03_DPCM)
                {
                    if (tr.device == DEV_2A03_DPCM && !dpcmToneSpecified)
                    {
                        dpcmDefaultRequired = true;
                    }
                    //nn = 0x0f - nn & 0x0f;    //ドライバ側でやる
                    nn = nn & 0x0f;
                }
                else
                {
                    nn += sweepStart;
                }

                //あまり大きくなり過ぎないようにする
                while (prevnotes.size() > 20)
                {
                    prevnotes.pop();
                }

                skipSpace();
                if (usePDelay)
                {
                    while ((int)prevnotes.size() > pddist)
                    {
                        prevnotes.pop();
                    }

                    calcLength(nn, data, lengthtbl, grace, pdlen, lengthError, lengthErrorScale, deflenIsFrames);

                    if (pdvol < 0)
                    {
                        data.push_back(TAI_SLUR);
                    }
                    else
                    {
                        data.push_back(VOLUME);
                        data.push_back(pdvol);
                    }

                    if (prevnotes.size() > 0)
                    {
                        if (pdlen == deflen)
                        {
                            data.push_back(prevnotes.front());
                        }
                        else
                        {
                            data.push_back(prevnotes.front() + 0x80);
                            data.push_back(timebase / pdlen);
                        }
                    }
                    else
                    {
                        if (pdlen == deflen)
                        {
                            data.push_back(nn);
                        }
                        else
                        {
                            data.push_back(nn + 0x80);
                            data.push_back(timebase / pdlen);
                        }
                    }

                    if (pdvol > 0)
                    {
                        //ボリュームを戻す
                        data.push_back(VOLUME);
                        data.push_back(volume);
                    }
                }
                else
                {
                    calcLength(nn, data, lengthtbl, grace, 0, lengthError, lengthErrorScale, deflenIsFrames);
                }
                prevnotes.push(nn);
            }
            break;
        case 'r':   //休符
        case 'R':
            skipSpace();
            if (isNextChar('-'))
            {
                data.push_back(ENV_DISABLE);            //エンベロープ無効
            }

            skipSpace();
            if (usePDelay)
            {
                while ((int)prevnotes.size() > pddist)
                {
                    prevnotes.pop();
                }

                calcLength(REST_DEFLEN, data, lengthtbl, grace, pdlen, lengthError, lengthErrorScale, deflenIsFrames, preserveDefaultRest);

                if (pdvol > 0)
                {
                    data.push_back(VOLUME);
                    data.push_back(pdvol);
                }

                if (prevnotes.size() > 0)
                {
                    if (pdlen == deflen)
                    {
                        data.push_back(prevnotes.front());
                    }
                    else
                    {
                        data.push_back(prevnotes.front() + 0x80);
                        data.push_back(timebase / pdlen);
                    }
                }
                else
                {
                    if (pdlen == deflen)
                    {
                        data.push_back(REST_DEFLEN);
                    }
                    else
                    {
                        data.push_back(REST_LENGTH);
                        data.push_back(timebase / pdlen);
                    }
                }

                if (pdvol > 0)
                {
                    //ボリュームを戻す
                    data.push_back(VOLUME);
                    data.push_back(volume);
                }
            }
            else
            {
                calcLength(REST_DEFLEN, data, lengthtbl, grace, 0, lengthError, lengthErrorScale, deflenIsFrames, preserveDefaultRest);
            }
            prevnotes.push(REST_DEFLEN);
            break;
        case '[':   //ループ開始
            skipSpace();
            if (getMultiDigit(n))
            {
                data.push_back(LOOP_START);
                data.push_back(n);
                isLooped = true;
            }
            break;
        case ']':   //ループ終了
            data.push_back(LOOP_END);
            isLooped = false;
            if (isLoopedMid)
            {
                //ループ途中終了時のパラメータを戻す
                volume = loopmid_volume;
                octave = loopmid_octave;
                tone = loopmid_tone;
				isLoopedMid = false;
            }
            break;
        case ':':   //ループ途中終了
            data.push_back(LOOP_MID_END);
			isLoopedMid = true;
            //ループ途中終了時のパラメータを保存
			loopmid_volume = volume;
			loopmid_octave = octave;
			loopmid_tone = tone;
            break;
        case 'q':   //ゲートタイムq
            skipSpace();
            if (getMultiDigit(n))
            {
                int gate;
                if (n == 0)
                {
                    gate = 0;
                }
                else
                {
                    gate = n | (1 << 6);
                }
                data.push_back(GATE_TIME);
                data.push_back(gate);
            }
            break;
        case 'u':   //ゲートタイムu
            skipSpace();
            if (getMultiDigit(n))
            {
                int gate;
                if (n == 0)
                {
                    gate = 0;
                }
                else
                {
                    gate = n | (2 << 6);
                }
                data.push_back(GATE_TIME);
                data.push_back(gate);
            }
            break;
        case 'Q':   //ゲートタイムQ
            skipSpace();
            if (getMultiDigit(n))
            {
                int gate;
                if (n == 0)
                {
                    gate = 0;
                }
                else
                {
                    gate = n | (3 << 6);
                }
                data.push_back(GATE_TIME);
                data.push_back(gate);
            }
            break;
        case 'k':   //相対キーシフト
            skipSpace();
            if (getMultiDigit(n))
            {
                data.push_back(KEYSHIFT_REL);
                data.push_back(n);
            }
            break;
        case 'K':   //絶対キーシフト
            skipSpace();
            if (getMultiDigit(n))
            {
                data.push_back(KEYSHIFT_ABS);
                data.push_back(n);
            }
            break;
        case '&':   //スラー
            data.push_back(TAI_SLUR);
            if (sweepStopBeforeSlur || slurTargetHasSweepStop())
            {
                data.push_back(SW_SWEEP_STOP);
                sweepStart = 0;
                if (!sweepStopBeforeSlur)
                {
                    normalizedSweepStops++;
                }
                sweepStopBeforeSlur = false;
            }
            break;
        case '@':   //音色指定か各種定義
            skipSpace();
            if (getMultiDigit(n))   //数値なら音色指定
            {
                if (tr.device == DEV_2A03_DPCM)     //DPCMトラックなら
                {
                    if (dpcmlist.count(n))
                    {
                        data.push_back(TONE);
                        data.push_back(dpcmlist[n].offset / 0x40);
                        data.push_back(dpcmlist[n].size / 0x10);
                        data.push_back(dpcmlist[n].init);
                        dpcmToneSpecified = true;
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : DPCM #" << n << " is not registered." << std::endl;
                        exit(1);
                    }
                }
                else if (tr.device >= DEV_SS5B_SQR1 && tr.device <= DEV_SS5B_SQR3)
                {
                    std::vector<int> t;
                    t.push_back(n);
                    int r = 0;

                    skipSpace();
                    while (getc(c))
                    {
                        if (c == ',')
                        {
                            skipSpace();
                            if (getMultiDigit(n))
                            {
                                t.push_back(n);
                            }
                        }
                        else
                        {
                            ss.seekg((int)ss.tellg() - 1);
                            break;
                        }
                        skipSpace();
                    }

                    if (t.size() >= 3)
                    {
                        for (int i = 0; i < 3; i++)
                        {
                            switch (t[i])
                            {
                            case 0:
                                r |= 0x08 << i; break;
                            case 1:
                                r |= 0x01 << i; break;
                            case 2:
                                break;
                            }
                        }
                        data.push_back(TONE);
                        data.push_back(r);
                        data.push_back(t[0]);
                        data.push_back(t[1]);
                        data.push_back(t[2]);
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : Missing tone arguments." << std::endl;
                        exit(1);
                    }
                }
                else
                {
                    if (tr.device >= DEV_VRC7_CH1 && tr.device <= DEV_VRC7_CH6 && (n < 0 || n > 15))
                    {
                        std::cerr << "Line " << linenum << " : VRC7 instrument number must be 0 to 15." << std::endl;
                        exit(1);
                    }
                    if (n == 0)
                    {
                        pushVrc7Patch(0);
                    }
                    if (tr.device >= DEV_N163_CH1 && tr.device <= DEV_N163_CH8)
                    {
                        pushN163WaveSetup(n);
                    }
                    data.push_back(TONE);
                    data.push_back(n);
					tone = n;   //音色を保存
                    if (tr.device == DEV_FDS)
                    {
                        fdsToneSpecified = true;
                    }
                }
            }
            else if (isNextStr("vrc7u") || isNextStr("VRC7U"))
            {
                if (!(expdevice & Expdev::VRC7) || tr.device < DEV_VRC7_CH1 || tr.device > DEV_VRC7_CH6)
                {
                    std::cerr << "Line " << linenum << " : @vrc7u can only be used in a VRC7 track." << std::endl;
                    exit(1);
                }
                skipSpace();
                if (!getMultiDigit(n) || n < 0 || n > 255)
                {
                    std::cerr << "Line " << linenum << " : VRC7 custom instrument number must be 0 to 255." << std::endl;
                    exit(1);
                }
                pushVrc7Patch(n);
                data.push_back(TONE);
                data.push_back(0);
                tone = 0;
            }
            else if (isNextStr("fds"))
            {
                if ((expdevice & Expdev::FDS) && tr.device == DEV_FDS)
                {
                    if (isNextChar('m'))   //FDSモジュレータ番号指定
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            if (n >= 0)
                            {
                                data.push_back(FDS_MOD_TONE);
                                data.push_back(n);
                                fdsModSpecified = true;
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : FDS mod number must be 0 or more" << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : FDS mod number is not specified." << std::endl;
                            exit(1);
                        }
					}
                    else if (isNextChar('e'))   //FDSモジュレーションエンベロープ指定
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            if (n >= 0 && n < 2)
                            {
                                int d = n;
								skipSpace();
                                if (isNextChar(','))
                                {
                                    if (getMultiDigit(n))
                                    {
                                        if (n >= 0 && n < 64)
                                        {
                                            data.push_back(FDS_MOD_ENV);
                                            data.push_back(n | d << 6);
                                        }
                                        else
                                        {
                                            std::cerr << "Line " << linenum << " : FDS envelope must be 0 to 63" << std::endl;
                                            exit(1);
                                        }
                                    }
                                    else
                                    {
                                        std::cerr << "Line " << linenum << " : FDS envelope value is not specified." << std::endl;
                                        exit(1);
                                    }
                                }
                                else
                                {
                                    std::cerr << "Line " << linenum << " : FDS envelope value is not specified." << std::endl;
                                    exit(1);
                                }
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : FDS envelope direction must be 0 or 1" << std::endl;
								exit(1);
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : FDS envelope direction is not specified." << std::endl;
                            exit(1);
                        }
                    }
                    else if (isNextChar('g'))   //FDSモジュレーションゲイン指定
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            if (n >= 0 && n < 64)
                            {
                                data.push_back(FDS_MOD_ENV);
                                data.push_back(n | 0x80);
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : FDS mod gain must be 0 to 63" << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : FDS mod gain is not specified." << std::endl;
                            exit(1);
                        }
                    }
                    else if (isNextChar('f'))   //FDSモジュレーション周波数指定
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            if (n >= 0 && n < 4096)
                            {
                                data.push_back(FDS_MOD_FREQ);
                                data.push_back(n & 0xff);
                                data.push_back(n >> 4);
                            }
                            else if (n < 0)
                            {
								data.push_back(FDS_MOD_FREQ);
								data.push_back(0x00);
                                data.push_back(0x80);
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : FDS mod freq must be 4095 or less" << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : FDS mod freq is not specified." << std::endl;
                            exit(1);
                        }
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : Invalid FDS command." << std::endl;
                        exit(1);
                    }
                }
            }
            else if (getc(c))
            {
                if (c == 'p' || c == 'P')       //指定した曲番号のデータを再生
                {
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        bool hasmusic = false;
                        for (const auto& music : musiclist)
                        {
                            if (music == n)
                            {
                                data.push_back(PLAY_DATA);
                                data.push_back(music);
                                hasmusic = true;
                            }
                        }

                        if (!hasmusic)
                        {
                            std::cerr << "Line " << linenum << " : Music #" << n << " is not registered." << std::endl;
                            exit(1);
                        }
                    }
                }
                else if (c == 'm' || c == 'M')       //ノートマップ定義
                {
                    if (isTrack || trheadsize == 0) //曲中かサブルーチン中にあったら指定
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            if (mapdiflist.count(n) > 0)
                            {
                                usingNoteMap = n;
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : Map diffinition #" << n << " is not registered." << std::endl;
                                exit(1);
                            }
                        }
                        else if (isNextChar('*'))
                        {
                            usingNoteMap = -1;
                        }
                    }
                    else
                    {
                        //それ以外は定義？
                        std::cerr << "Line " << linenum << " : Invalid map diffinition." << std::endl;
                        exit(1);
                    }
                }
                else if (c == 'd' || c == 'D')   //デチューン
                {
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        data.push_back(DETUNE);
                        data.push_back(-n);
                    }
                    else
                    {
                        if (isNextChar('-'))
                        {
                            if (getMultiDigit(n))
                            {
                                data.push_back(DETUNE);
                                data.push_back(n);
                            }
                        }
                    }
                }
                else if (c == 'v' || c == 'V')   //音量エンベロープ
                {
                    skipSpace();
                    getAndPushEnvAssign(data, envdata, VOLUME_ENV, v_offset);
                    break;
                }
                else if (c == 'f' || c == 'F')   //音程エンベロープ
                {
                    skipSpace();
                    if (isNextChar('s') || isNextChar('S'))
                    {
                        skipSpace();
                        if (getMultiDigit(n) && n >= 0 && n <= 7)
                        {
                            data.push_back(FREQ_ENV_SHIFT);
                            data.push_back(n);
                            break;
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : Frequency envelope shift must be 0 to 7." << std::endl;
                            exit(1);
                        }
                    }
                    if (tr.device >= DEV_VRC7_CH1 && tr.device <= DEV_VRC7_CH6)
                    {
                        getAndPushEnvAssign(data, vrc7FreqEnvdata, FREQ_ENV, f_offset);
                    }
                    else
                    {
                        getAndPushEnvAssign(data, envdata, FREQ_ENV, f_offset);
                    }
                    break;
                }
                else if (c == 'n' || c == 'N')   //ノートエンベロープ
                {
                    skipSpace();
                    getAndPushEnvAssign(data, envdata, NOTE_ENV, n_offset);
                    break;
                }
                else if (c == 't' || c == 'T')   //音色エンベロープ
                {
                    skipSpace();
                    getAndPushEnvAssign(data, envdata, TONE_ENV, t_offset);
                    break;
                }
                else
                {
                    ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
                }
            }
            break;
        case 'l':   //デフォルト音長
            skipSpace();
            if (getMultiDigit(n))
            {
                deflen = n;
                deflenIsFrames = false;
                for (int i = 0, size = (int)sizeof(nthnotetbl); i < size; i++)
                {
                    if (deflen == nthnotetbl[i])
                    {
                        data.push_back(DEF_LENGTH);
                        data.push_back(lengthtbl[i]);
                        break;
                    }
                }
            }
            else if (isNextChar('%'))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    deflen = n;
                    deflenIsFrames = true;
                    data.push_back(DEF_LENGTH);
                    data.push_back(n);
                }
            }
            break;
        case 'L':   //無限ループ
            data.push_back(INF_LOOP);
            break;
        case 'o':   //オクターブ
        case 'O':
            skipSpace();
            if (getMultiDigit(n))
            {
                octave = n;
            }
            break;
        case '>':   //オクターブ上げ
            octave += (octaveReverse) ? -1 : 1;
            break;
        case '<':   //オクターブ下げ
            octave += (octaveReverse) ? 1 : -1;
            break;
        case 'v':   //トラックボリューム指定
        case 'V':
            skipSpace();
            getc(c);
            if (c == '+')           //相対指定+
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    if (n > 15)
                    {
                        n = 15;
                    }
                    data.push_back(VOLUME);
                    data.push_back(0x10 | n);
                    volume += n;
                }
            }
            else if (c == '-')           //相対指定-
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    if (n > 15)
                    {
                        n = 15;
                    }
                    data.push_back(VOLUME);
                    data.push_back(-n);
                    volume -= n;
                }
            }
            else if (isDigit(c))
            {
                ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す

                if (getMultiDigit(n))    //絶対指定
                {
                    if (n > 15)
                    {
                        n = 15;
                    }
                    data.push_back(VOLUME);
                    data.push_back(n);
                    volume = n;
                }
            }
            break;
        case 'n':
        case 'N':
            if (isNextStr("163ch"))
            {
                if (trheadsize == 0 || isTrack)
                {
                    std::cerr << "Line " << linenum << " : N163CH must be specified before the first Track in Music." << std::endl;
                    exit(1);
                }
                skipSpace();
                if (!getMultiDigit(n) || n != n163ChannelCount)
                {
                    std::cerr << "Line " << linenum << " : Invalid N163CH." << std::endl;
                    exit(1);
                }
            }
            else
            {
                std::cerr << "Line " << linenum << " : Unknown command '" << c << "'" << std::endl;
                exit(1);
            }
            break;
        case 't':   //トラック開始かテンポ
        case 'T':
            if (isNextStr("rack"))   //トラック開始
            {
                //サブルーチン中
                if (trheadsize == 0)
                {
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        tr.track = n;
                    }

                    skipSpace();
                    if (isNextChar(','))
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            tr.device = n;
                        }
                    }
                }
                else
                {
                    if (!isTrack)
                    {
                        isTrack = true;
                    }
                    else
                    {
                        usingCmds.clear();  //マップ関係の変数をここでリセットする
                        usingNoteMap = -1;

                        usePDelay = false;  //疑似ディレイもリセット

                        setDpcmDefault();
                        setFdsDefaults();
                        data.push_back(TRACK_END);    //終了コード
                        tr.data = data;
                        tracks.push_back(tr);
                        data.clear();
                        deflen = 4;
                        deflenIsFrames = false;
                        lengthError = 0;
                        lengthErrorScale = 192;
                        dpcmToneSpecified = false;
                        dpcmDefaultRequired = false;
                        fdsToneSpecified = false;
                        fdsModSpecified = false;
                        n163WaveSetup = -1;
                    }

                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        if (n < 0 && n > 15)
                        {
                            std::cerr << "Line " << linenum << " : Please set the track number from 0 to 15." << std::endl;
                            exit(1);
                        }
                        tr.track = n;
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : No track number." << std::endl;
                        exit(1);
                    }

                    skipSpace();
                    if (isNextChar(','))
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            tr.device = n;

                            if (tr.device >= DEV_VRC7_CH1 && tr.device <= DEV_VRC7_CH6 && !(expdevice & Expdev::VRC7))
                            {
                                std::cerr << "Line " << linenum << " : VRC7 track requires #expdevice VRC7." << std::endl;
                                exit(1);
                            }

                            if (tr.device >= DEV_N163_CH1 && tr.device <= DEV_N163_CH8)
                            {
                                if (!(expdevice & Expdev::N163))
                                {
                                    std::cerr << "Line " << linenum << " : N163 track requires #expdevice N163." << std::endl;
                                    exit(1);
                                }
                                if (tr.device >= DEV_N163_CH1 + n163ChannelCount)
                                {
                                    std::cerr << "Line " << linenum << " : N163 track exceeds N163CH " << n163ChannelCount << "." << std::endl;
                                    exit(1);
                                }
                            }

                            if (tr.device == DEV_FDS)
                            {
                                data.push_back(HW_ENV);
								data.push_back(0x80); //FDSのハードウェアエンベロープ無効化
                            }
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : No device number." << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "Line " << linenum << " : No device number." << std::endl;
                        exit(1);
                    }
                }
            }
            else if (getMultiDigit(n))    //テンポ
            {
                if (!isTrack)
                {
                    std::cerr << "Line " << linenum << " : Prease write the tempo in the track." << std::endl;
                    exit(1);
                }
                constexpr int TEMPO_DENOMINATOR = 14400;
                long long tickRateNumerator = static_cast<long long>(n) * timebase;
                long long tempoStep;
                int dir;

                if (tickRateNumerator < TEMPO_DENOMINATOR)
                {
                    dir = 0;
                    tempoStep = TEMPO_DENOMINATOR - tickRateNumerator;
                }
                else
                {
                    dir = 1;
                    tempoStep = tickRateNumerator - TEMPO_DENOMINATOR;
                }

                // The driver supports from 0 to 2 ticks per video frame.
                if (tempoStep > TEMPO_DENOMINATOR)
                {
                    tempoStep = TEMPO_DENOMINATOR;
                }

                data.push_back(TEMPO);
                data.push_back(dir);
                data.push_back(tempoStep & 0xff);
                data.push_back((tempoStep >> 8) & 0xff);
            }
            break;
        case 's':   //ソフトウェアスイープ
        case 'S':   //ソフトウェアスイープ
        {
                int start, end, delay, speed;
                bool res = false;
                skipSpace();
                getc(c);
                if (c == '*')   //解除
                {
                    if (normalizedSweepStops > 0)
                    {
                        normalizedSweepStops--;
                    }
                    else if (nextTokenIsSlur())
                    {
                        sweepStopBeforeSlur = true;
                    }
                    else
                    {
                        data.push_back(SW_SWEEP_STOP);
                    }
                    sweepStart = 0;
                }
                else if (isDigit(c) || c == '-')
                {
                    ss.seekg((int)ss.tellg() - 1);

                    if (getMultiDigit(start))
                    {
                        skipSpace();
                        getc(c);
                        if (c == ',' && getMultiDigit(end))
                        {
                            skipSpace();
                            getc(c);
                            if (c == ',' && getMultiDigit(delay))
                            {
                                skipSpace();
                                getc(c);
                                if (c == ',' && getMultiDigit(speed))
                                {
                                    unsigned char arg1, arg2;
                                    if (!packSoftwareSweep(start, end, delay, speed, arg1, arg2))
                                    {
                                        std::cerr << "Line " << linenum << " : Software sweep argument is out of range." << std::endl;
                                        exit(1);
                                    }
                                    data.push_back(SW_SWEEP);
                                    data.push_back(arg1);
                                    data.push_back(arg2);
                                    sweepStart = start;
                                    res = true;
                                }
                            }
                        }
                    }
                    if (!res)
                    {
                        std::cerr << "Line " << linenum << " : Missing softwear sweep arguments." << std::endl;
                        exit(1);
                    }
                }
            }
            break;
        case 'p':   //疑似ディレイ
        case 'P':
            if (isNextChar('d'))
            {
                bool res = false;
                skipSpace();
                if (isNextChar('*'))
                {
                    usePDelay = false;
                    res = true;
                }
                else if (getMultiDigit(n))
                {
                    pddist = n;
                    skipSpace();
                    if (isNextChar(','))
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            pdvol = n;
                            skipSpace();
                            if (isNextChar(','))
                            {
                                skipSpace();
                                if (getMultiDigit(n))
                                {
                                    pdlen = n;
                                    usePDelay = true;
                                    res = true;
                                }
                            }
                        }
                    }
                }

                if (!res)
                {
                    std::cerr << "Line " << linenum << " : Missing psuedo delay arguments." << std::endl;
                    exit(1);
                }
            }
            break;
        case 'w':   //メモリ書き込み
        case 'W':
            if (getMultiDigit(n))
            {
                int address, value;
                bool res = false;
                address = n;
                skipSpace();
                if (isNextChar(','))
                {
                    skipSpace();
                    if (getMultiDigit(n))
                    {
                        value = n;
                        data.push_back(MEM_WRITE);
                        data.push_back(address & 0xff);
                        data.push_back(address >> 8);
                        data.push_back(value & 0xff);
                        res = true;
                    }
                }

                if (!res)
                {
                    std::cerr << "Line " << linenum << " : Missing Memory Write arguments." << std::endl;
                    exit(1);
                }
            }
            break;
        case 'h':   //ハードウェア命令
        case 'H':
            getc(c);
            if (c == 's' || c == 'S')   //ハードウェアスイープ
            {
                int rate, dir, amount;
                bool res = false;
                skipSpace();
                if (isNextChar('*'))
                {
                    data.push_back(HW_SWEEP);
                    data.push_back(0x08);
                    res = true;
                }
                else if (getMultiDigit(n))
                {
                    rate = n;
                    skipSpace();
                    if (isNextChar(','))
                    {
                        skipSpace();
                        if (getMultiDigit(n))
                        {
                            skipSpace();
                            if (isNextChar(','))
                            {
                                dir = n;
                                skipSpace();
                                if (getMultiDigit(n))
                                {
                                    amount = n;
                                    unsigned char v =
                                        0x80 +
                                        (unsigned char)((rate & 0x07) << 4) +
                                        (unsigned char)((dir & 0x01) << 3) +
                                        ((unsigned char)amount & 0x07);
                                    data.push_back(HW_SWEEP);
                                    data.push_back(v);
                                    res = true;
                                }
                            }
                        }
                    }
                }

                if (!res)
                {
                    std::cerr << "Line " << linenum << " : Missing hardwear sweep arguments." << std::endl;
                    exit(1);
                }
            }
            else if (c == 'e' || c == 'E')  //ハードウェアエンベロープ
            {
                bool res = false;

                if (tr.device >= DEV_SS5B_SQR1 && tr.device <= DEV_SS5B_SQR3)
                {
                    int rate, shape;
                    skipSpace();
                    if (isNextChar('*'))
                    {
                        data.push_back(HW_ENV);
                        data.push_back(0x00);
                        res = true;
                    }
                    else if (getMultiDigit(n))
                    {
                        rate = n;
                        skipSpace();
                        if (isNextChar(','))
                        {
                            skipSpace();
                            if (getMultiDigit(n))
                            {
                                shape = n;
                                data.push_back(HW_ENV);
                                data.push_back(0xff & rate);
                                data.push_back((rate >> 8) & 0xff);
                                data.push_back(0xf & shape);
                                res = true;
                            }
                        }
                    }
                }
                else if (tr.device == DEV_FDS)
                {
                    int rate;
                    skipSpace();
                    if (isNextChar('*'))
                    {
                        data.push_back(HW_ENV);
                        data.push_back(0x80);
                        res = true;
                    }
                    else if (getMultiDigit(n))
                    {
                        rate = n;
                        unsigned char v = (unsigned char)(rate & 0x3f);
                        data.push_back(HW_ENV);
                        data.push_back(v);
                        res = true;
                    }
                }
                else
                {
                    int rate, loop;
                    skipSpace();
                    if (isNextChar('*'))
                    {
                        data.push_back(HW_ENV);
                        data.push_back(0x10);
                        res = true;
                    }
                    else if (getMultiDigit(n))
                    {
                        loop = n;
                        skipSpace();
                        if (isNextChar(','))
                        {
                            skipSpace();
                            if (getMultiDigit(n))
                            {
                                rate = n;
                                unsigned char v = (unsigned char)((loop & 0x01) << 5) + (unsigned char)(rate & 0x0f);
                                data.push_back(HW_ENV);
                                data.push_back(v);
                                res = true;
                            }
                        }
                    }
                }
                
                if (!res)
                {
                    std::cerr << "Line " << linenum << " : Missing hardwear envelope arguments." << std::endl;
                    exit(1);
                }
            }
            break;
        case '\\':   //サブルーチン
            skipSpace();
            if (getMultiDigit(n))
            {
                if (subdata.count(n))
                {
                    data.push_back(SUBROUTINE);
                    data.push_back(subdata[n].addr & 0x00ff);
                    data.push_back((subdata[n].addr & 0xff00) >> 8);
                    isLooped = true;
					tone = subdata[n].tone;
                }
            }
            break;
        case '}':   //終了
            if (trheadsize > 0)
            {
                if (isTrack)
                {
                    isTrack = false;
                    setDpcmDefault();
                    setFdsDefaults();
                    data.push_back(TRACK_END);    //終了コード
                    tr.data = data;
                    tracks.push_back(tr);
                }

                std::sort(tracks.begin(), tracks.end());

                //曲が終了したのでヘッダ作成
                //トラック数→1トラックのアドレス→1トラックのトラック番号→1トラックの音源番号→2トラックのアドレス→2トラックのトラック番号…
                //デフォ音長の順
                int pos = totalpos + trheadsize;
                trhead.clear();
				trbody.clear();

                for (const auto& t : tracks)
                {
                    trhead.push_back(t.track);          //トラック番号
                    trhead.push_back(t.device);         //トラックの音源番号
                    trhead.push_back(pos & 0x00ff);        //トラック開始アドレスの下位バイト
                    trhead.push_back((pos & 0xff00) >> 8); //トラック開始アドレスの上位バイト
                    pos += t.data.size();
                }

                for (const auto& t : tracks)
                {
                    std::copy(t.data.begin(), t.data.end(), std::back_inserter(trbody));
                }
            }
            else //トラックヘッダがない＝サブルーチンの場合
            {
                std::copy(data.begin(), data.end(), std::back_inserter(trbody));
            }
            return; //1曲ずつ読むのでここでreturnする
        default:
            std::cerr << "Line " << linenum << " : Unknown command '" << c << "'" << std::endl;
            exit(1);
            break;
        }
    }

    //閉じカッコが足りない
    std::cerr << "Missing }." << std::endl;
    exit(1);
}


bool MMLReader::skipUntil(std::string input)
{
    char c;

    while (ss.get(c))
    {
        for (int i = 0; i < (int)input.size(); i++)
        {
            if (c == input[i])
            {
                if (i == (int)input.size() - 1)     //全部一致した
                {
                    return true;
                }
                else
                {
                    if (!ss.get(c))                //途中で終端に達した
                    {
                        return false;
                    }
                }
            }
            else
            {
                break;
            }
        }
        
        if (c == '\n')
        {
            linenum++;
        }
    }
    return false;   //一致せず終端に達した
}


bool MMLReader::isDigit(char c)
{
    return c >= '0' && c <= '9';
}


bool MMLReader::isHex(char c)
{
    return c >= '0' && c <= '9' || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f';
}


bool MMLReader::skipSpace()
{
    char c;

    while (ss.get(c))
    {
        if (c == '\n')
        {
            linenum++;
        }
        else if (c != ' ' && c != '　' && c != '\t' && c != '\r')
        {
            ss.seekg((int)ss.tellg() - 1);
            return true;
        }
    }
    return false;   //一致せず終端に達した
}


bool MMLReader::skipComment()
{
    char c;

    while (ss.get(c))
    {
        if (c == '/')
        {
            getc(c);
            if (c == '/')
            {
                if (!skipUntil("\n"))
                {
                    std::cerr << "No end of comment." << std::endl;
                    exit(1);
                }
                ss.seekg((int)ss.tellg() - 1);
                return true;
            }
            else if (c == '*')
            {
                if (!skipUntil("*/"))
                {
                    std::cerr << "No end of comment." << std::endl;
                    exit(1);
                }
                return true;
            }
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);
            return true;
        }

        if (c == '\n')
        {
            linenum++;
        }
    }
    return false;
}


bool MMLReader::skipSpaceUntilNextLine()
{
    char c;

    while (ss.get(c))
    {
        if (c == '\n')
        {
            ss.seekg((int)ss.tellg() - 1);
            return true;
        }
        else if (c != ' ' && c != '　' && c != '\t' && c != '\r')
        {
            ss.seekg((int)ss.tellg() - 1);
            return true;
        }
    }
    return false;   //一致せず終端に達した
}


bool MMLReader::findStr(std::string str, std::string exclude)
{
    char c;
    size_t pos = 0;
    size_t posex = 0;

    while (ss.get(c))
    {
        if (c == '\n')
        {
            linenum++;
        }

        if (tolower(c) == tolower(exclude[posex]))
        {
            if (posex >= exclude.size() - 1)
            {
                return false;
            }
            posex++;
        }
        else if (posex > 0)
        {
            //相違があった文字がまた1文字目と一致する可能性があるので戻す
            ss.seekg((int)ss.tellg() - 1);
            posex = 0;
        }
        else
        {
            posex = 0;
        }

        if (tolower(c) == tolower(str[pos]))
        {
            if (pos >= str.size() - 1)
            {
                return true;
            }
            pos++;
        }
        else if (pos > 0)
        {
            //相違があった文字がまた1文字目と一致する可能性があるので戻す
            ss.seekg((int)ss.tellg() - 1);
            pos = 0;
        }
        else
        {
            pos = 0;
        }
        skipComment();
    }
    return false;
}


bool MMLReader::findStr(std::string str)
{
    char c;
    size_t pos = 0;
    while (ss.get(c))
    {
        if (c == '\n')
        {
            linenum++;
        }

        if (tolower(c) == tolower(str[pos]))
        {
            if (pos >= str.size() - 1)
            {
                return true;
            }
            pos++;
        }
        else if (pos > 0)
        {
            //相違があった文字がまた1文字目と一致する可能性があるので戻す
            ss.seekg((int)ss.tellg() - 1);
            pos = 0;
        }
        else
        {
            pos = 0;
        }
        skipComment();
    }
    return false;
}

//次に来る文字が指定した文字と同じかどうか判定する
bool MMLReader::isNextChar(char input)
{
    char c;
    getc(c);
    if (tolower(c) == tolower(input))
    {
        return true;
    }
    else
    {
        ss.seekg((int)ss.tellg() - 1);
        return false;
    }
}

//次に来る文字列が指定した文字列と同じかどうか判定する
bool MMLReader::isNextStr(std::string str)
{
    char c;
    size_t pos = 0;
    while (ss.get(c))
    {
        if (tolower(c) == tolower(str[pos]))
        {
            if (pos >= str.size() - 1)
            {
                return true;
            }
            pos++;
        }
        else
        {
            ss.seekg((int)ss.tellg() - pos - 1);    //読み込み位置を戻す
            return false;
        }
    }
    return false;
}

//ストリームが終了したらexitする関数
bool MMLReader::getc(char& c)
{
    if (ss.get(c))
    {
        return true;
    }
    return false;
}


void MMLReader::syntaxErr()
{
    std::cerr << "Line " << linenum << " : Syntax error." << std::endl;
    exit(1);
}


bool MMLReader::getMultiDigit(int& res)
{
    char c;
    std::string d;
    int base = 10;

    skipSpace();

    if (getc(c))
    {
        if (c == '-')
        {
            d += c;
            if (!getc(c))
            {
                return false;
            }
        }

        if (c == '$')
        {
            base = 16;
        }
        else if (c == '%')
        {
            base = 2;
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);
        }
    }

    const size_t signLength = d.size();
    while (getc(c))
    {
        const bool validDigit = base == 16 ? isHex(c) :
            base == 2 ? c == '0' || c == '1' : isDigit(c);
        if (validDigit)
        {
            d += c;
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
            break;
        }
    }

    if (d.size() > signLength)
    {
        res = std::stoi(d, nullptr, base);
        return true;
    }
    else
    {
        return false;
    }
}

bool MMLReader::getStrInQuote(std::string& str)
{
    char c;
    bool isquote = false;
    skipSpace();
    while (getc(c))
    {
        if (!isquote && c == '"')
        {
            isquote = true;
        }
        else if (isquote && c == '"')
        {
            return true;
        }
        else if (isquote && c == '\n')
        {
            linenum++;
            return true;
        }
        else if (isquote)
        {
            str += c;
        }
    }
    return false;
}

bool MMLReader::getByte(int& res)
{
    char c;
    std::string d;

    for (int i = 0; i < 2; i++)
    {
        getc(c);

        if (isHex(c))
        {
            d += c;
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);
            break;
        }
    }

    if (!d.empty())
    {
        res = std::stoi(d, nullptr, 16);
        return true;
    }
    else
    {
        return false;
    }
}


void MMLReader::calcLength(char cmd, std::vector<unsigned char>& data, std::vector<unsigned char> lengthtbl, int& prevGrace, int shorten,
    long long& lengthError, long long& lengthErrorScale, bool deflenIsFrames, bool preserveDefaultRest)
{
    char c;
    int n = 0;
    int frames = 0;
    int dottedDenominator = 0;
    bool dotted = false;
    bool envdis = false;
    bool framelen = false;
    bool grace = false;
    bool joinedRest = false;
    int lengthCorrection = 0;
    std::string digit;

    auto calcNthLength = [&](int denominator)
    {
        int length = timebase / denominator;
        int remainder = timebase % denominator;

        // Extend the common scale when dots introduce denominators such as
        // 128 or 256, which do not divide the base scale of 192.
        long long commonScale = std::lcm(lengthErrorScale, static_cast<long long>(denominator));
        lengthError *= commonScale / lengthErrorScale;
        lengthError += remainder * (commonScale / denominator);
        lengthCorrection += static_cast<int>(lengthError / commonScale);
        lengthError %= commonScale;
        lengthErrorScale = commonScale;

        long long divisor = std::gcd(lengthError, lengthErrorScale);
        lengthError /= divisor;
        lengthErrorScale /= divisor;

        return length;
    };

    auto nextRestHasExplicitLength = [&]()
    {
        std::string src = ss.str();
        size_t pos = static_cast<size_t>(ss.tellg());

        while (pos < src.size())
        {
            char next = src[pos];
            if (next == ' ' || next == '\t' || next == '\r' || next == '\n')
            {
                pos++;
                continue;
            }

            if (next == '/' && pos + 1 < src.size() && src[pos + 1] == '/')
            {
                pos += 2;
                while (pos < src.size() && src[pos] != '\n')
                {
                    pos++;
                }
                continue;
            }

            if (next == '/' && pos + 1 < src.size() && src[pos + 1] == '*')
            {
                pos += 2;
                while (pos + 1 < src.size() && !(src[pos] == '*' && src[pos + 1] == '/'))
                {
                    pos++;
                }
                if (pos + 1 < src.size())
                {
                    pos += 2;
                }
                continue;
            }

            return isDigit(next) || next == '%' || next == '~';
        }

        return false;
    };

    while (getc(c))
    {
        if (c == '^' || (cmd == REST_DEFLEN && (c == 'r' || c == 'R')))      //タイ、もしくは休符が連続で来たとき
        {
            bool currentRestIsDefault = frames == 0 && digit.empty() && n == 0 && !dotted && !framelen;
            if (preserveDefaultRest && cmd == REST_DEFLEN && (c == 'r' || c == 'R') &&
                (currentRestIsDefault || !nextRestHasExplicitLength()))
            {
                if (!currentRestIsDefault)
                {
                    if (dotted)
                    {
                        frames += n;
                    }
                    else if (digit.empty())
                    {
                        frames += n;
                        frames += calcNthLength(deflen);
                    }
                    else
                    {
                        frames += n;
                        frames += calcNthLength(std::atoi(digit.c_str()));
                    }
                }
                ss.seekg((int)ss.tellg() - 1);
                break;
            }

            if (dotted)
            {
                frames += n;
            }
            else if (digit.empty())    //前に数値がない場合デフォ音長
            {
                frames += n;
                frames += calcNthLength(deflen);
            }
            else
            {
                frames += n;
                frames += calcNthLength(std::atoi(digit.c_str()));
            }
            dottedDenominator = 0;
            dotted = false;
            digit.clear();
            n = 0;
            joinedRest = (cmd == REST_DEFLEN && (c == 'r' || c == 'R'));
        }
        else if (c == '.')   //付点
        {
            if (dotted)        //多重付点
            {
                dottedDenominator *= 2;
                n += calcNthLength(dottedDenominator);
            }
            else if (digit.empty())    //前に数値がない場合デフォ音長
            {
                n = calcNthLength(deflen);
                dottedDenominator = deflen * 2;
                n += calcNthLength(dottedDenominator);
            }
            else
            {
                int denominator = std::atoi(digit.c_str());
                n = calcNthLength(denominator);
                dottedDenominator = denominator * 2;
                n += calcNthLength(dottedDenominator);
            }
            dotted = true;
            digit.clear();
        }
        else if (cmd == 0x6c && c == '-' && digit.empty())   //エンベロープ無効（休符のみ）
        {
            envdis = true;
        }
        else if (c == '%' && digit.empty())   //フレーム音長
        {
            framelen = true;
        }
        else if (c == '~' && digit.empty())  //装飾符
        {
            framelen = true;
            grace = true;
        }
        else if (isDigit(c))
        {
            if (c == '0')
            {
                std::cerr << "Line " << linenum << " : Note length is 0." << std::endl;
                exit(1);
            }
            digit += c;
        }
        else
        {
            if (!digit.empty())
            {
                if (framelen)
                {
                    frames = std::atoi(digit.c_str());
                }
                else
                {
                    n += calcNthLength(std::atoi(digit.c_str()));
                }
            }
            frames += n;
            if (joinedRest && digit.empty() && n == 0 && !dotted && !framelen)
            {
                frames += calcNthLength(deflen);
            }
            ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
            break;
        }

        skipSpace();
        skipComment();
        skipSpace();
    }

    if (envdis)            //エンベロープ無効
    {
        data.push_back(0xf8);
    }

    // A note without an explicit length is normally emitted using the current
    // default-length command. Emit an explicit length only when accumulated
    // error adds a tick; otherwise preserve the compact existing encoding.
    if (frames == 0 && !framelen && !deflenIsFrames)
    {
        int defaultLength = calcNthLength(deflen);
        if (lengthCorrection > 0)
        {
            frames = defaultLength;
        }
    }
    frames += lengthCorrection;

    if (prevGrace > 0)     //前の音符が装飾符
    {
        if (!grace)  //この音符も装飾符なら何もしない
        {
            if (frames == 0)
            {
                //デフォ音長を音長付きノートに直す
                frames = timebase / deflen;
            }

            frames -= prevGrace;
            prevGrace = 0;

            if (frames == timebase / deflen)
            {
                //デフォ音長と同じになったらデフォ音長に直す
                frames = 0;
            }
        }

        if (frames < 0)
        {
            std::cerr << "Line " << linenum << " : Note length has become less than 0." << std::endl;
            exit(1);
        }
    }

    if (shorten > 0)
    {
        if (frames == 0)
        {
            //デフォ音長を音長付きノートに直す
            frames = timebase / deflen;
        }

        frames -= timebase / shorten;

        if (frames == timebase / deflen)
        {
            //デフォ音長と同じになったらデフォ音長に直す
            frames = 0;
        }
    }

    if (frames > 0)
    {
        if (grace)
        {
            prevGrace += frames;     //次の音符から引く値
        }
        //音長付きノート
        while (true)
        {
            data.push_back(cmd + 0x80);
            if (frames > 0xff) // 1バイト超えたら分割
            {
                data.push_back(0xff);
                data.push_back(TAI_SLUR);
                frames -= 0xff;
            }
            else
            {
                data.push_back(frames);
                break;
            }
        }
    }
    else
    {
        data.push_back(cmd);
    }
}


void MMLReader::pushEnvAssign(std::vector<unsigned char>& data, std::map<int, EnvData>& envdata, int envtype, int envnum, int delay, bool ismap, int offset)
{
    envnum += offset;

    if (envdata.count(envnum))
    {
        data.push_back(envtype);
        data.push_back(envdata[envnum].addr & 0x00ff);           //エンベロープアドレスの下位バイト
        data.push_back((envdata[envnum].addr & 0xff00) >> 8);    //エンベロープアドレスの上位バイト
        data.push_back(delay);                              //ディレイ値
    }
    else
    {
        if (ismap)
        {
            std::cerr << "Line " << linenum << " : [Map difinition] Invalid envelope number " << envnum << "." << std::endl;
        }
        else
        {
            std::cerr << "Line " << linenum << " : Invalid envelope number " << envnum << "." << std::endl;
        }
        exit(1);
    }
}


void MMLReader::getAndPushEnvAssign(std::vector<unsigned char>& data, std::map<int, EnvData>& envdata, int envtype, int offset)
{
    int n;
    if (isNextChar('*'))
    {
        data.push_back(envtype + 1);     //停止コマンド
    }
    else
    {
        if (getMultiDigit(n))
        {
            int envn = n + offset;
            int delay = 0;
            skipSpace();
            if (isNextChar(','))
            {
                if (getMultiDigit(n))
                {
                    delay = n;
                }
            }

            pushEnvAssign(data, envdata, envtype, envn, delay, false, 0);
        }
    }
}


void MMLReader::getCmdArgs(CommandArgs& args)
{
    int n;
    skipSpaceUntilNextLine();
    if (getMultiDigit(n))       //コマンド内容を保存
    {
        args.push_back(n);

        skipSpaceUntilNextLine();
        if (isNextChar(','))
        {
            if (getMultiDigit(n))
            {
                args.push_back(n);
            }
        }
    }
}


bool MMLReader::getNoteNumber(int& nn)
{
    char c;
    int n;
    int tmp = 0;
    int note = 0;
    int pos = (int)ss.tellg();

    getc(c);

    switch (c)
    {
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
        tmp = 0x20;
		[[fallthrough]];
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
        note = std::clamp(c - tmp - 0x61, 0, 7);
        nn = notetbl[note];
        break;
    default:
        ss.seekg((int)pos);
        return false;
    }

    skipSpace();
    while (getc(c))          //半音
    {
        if (c == '+')
        {
            nn += 1;
        }
        else if (c == '-')
        {
            nn -= 1;
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
            break;
        }
        skipSpace();
    }

    if (getMultiDigit(n))
    {
        nn += n * 12;
    }
    else
    {
        ss.seekg((int)pos);
        return false;
    }
    return true;
}

//音長→フレームの変換テーブルを作成する
void MMLReader::makeLengthTbl(std::vector<unsigned char> &lengthtbl)
{
    for (int i = 0; i < sizeof(nthnotetbl); i++)
    {
        unsigned char n = timebase / nthnotetbl[i];
        if (n == 0)
        {
            n = 1;
        }
        lengthtbl.push_back(n);
    }
}
