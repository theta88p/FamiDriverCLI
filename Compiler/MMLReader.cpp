#include "MMLReader.h"

//ノートテーブル（ノート文字と音階の関係）
                      /* a b  c d e f g */
const unsigned char notetbl[] = { 9, 11, 0, 2, 4, 5, 7 };
//音長テーブル（音長番号と192/n音符の関係）
/* 1 2 3 4 6 8 12 16 24 32 48 64 */
const unsigned char nthnotetbl[] = { 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96 };

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
    extdevice = 0;
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
    char c;
    int n;
    int music = 0;
    std::map<std::string, std::string, cmpByStringLength> macrolist;
    std::vector<unsigned char> head;
    std::vector<unsigned char> body;
    std::vector<unsigned char> fds_wavdata;
	std::vector<unsigned char> fds_moddata;
	int fds_wavaddr = 0;
    int fds_modaddr = 0;
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

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す
    
    if (extdevice & ExtDev::FDS)
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

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    int envsize = 0;
    readEnvelope(envsize);
    totalpos += envsize;

    for (const auto& [k, v] : envdata)
    {
        std::copy(v.data.begin(), v.data.end(), std::back_inserter(body));
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    int subsize = 0;
    readSubRoutine(subsize);
    totalpos += subsize;

    for (const auto& [k ,v] : subdata)
    {
        std::copy(v.data.begin(), v.data.end(), std::back_inserter(body));
    }

    linenum = 1;
    ss.clear();
    ss.seekg(0);    //読み込み位置を戻す

    for (int i = 0; i < music; i++)
    {
		std::vector<unsigned char> trhead;
        std::vector<unsigned char> musdata;
        findStr("music");
        skipSpace();
        if (getMultiDigit(n) && n == musiclist[i])
        {
            if (findStr("{"))
            {
                int seek = ss.tellg();
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
                if (extdevice & ExtDev::FDS)
                {
					trheadsize += 4; //FDSの波形アドレス分
                }

                int tone = 0;
                readBrackets(ss.tellg(), trheadsize, trhead, musdata, tone);
                trhead.push_back(0xff);             //トラックヘッダ終端
                trhead.push_back(lengthtbl[3]);     //デフォルトのデフォルト音長（4分音符）

                if (extdevice & ExtDev::FDS)
                {
                    trhead.push_back(fds_wavaddr & 0xff); //FDSの波形アドレス
                    trhead.push_back(fds_wavaddr >> 8);
                    trhead.push_back(fds_modaddr & 0xff); //FDSのモジュレータ波形アドレス
                    trhead.push_back(fds_modaddr >> 8);
                }

                std::copy(trhead.begin(), trhead.end(), std::back_inserter(body));
                std::copy(musdata.begin(), musdata.end(), std::back_inserter(body));
                head.push_back(totalpos & 0xff);
                head.push_back(totalpos >> 8);
                totalpos += musdata.size();

                linenum = 1;
                ss.clear();
                ss.seekg(0);    //読み込み位置を戻す
            }
        }
    }


    std::copy(head.begin(), head.end(), std::back_inserter(seqdata));
    std::copy(body.begin(), body.end(), std::back_inserter(seqdata));

    ifs.close();
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
            if (isMusic || isTrack)
            {
                std::cerr << "Line " << linenum << " : Please write settings starting with '#' at the beginning of the file." << std::endl;
                exit(1);
            }
            else if (isNextStr("timebase"))
            {
                skipSpace();
                if (getMultiDigit(n))
                {
                    timebase = n;
                    lengthtbl.clear();
                    makeLengthTbl(lengthtbl);
                }
            }
            else if (isNextStr("offsetpcm"))
            {
                skipSpace();
                if (getMultiHex(n))
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
            else if (isNextStr("extdevice"))
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
                    extdevice |= ExtDev::VRC6;
                }

                if (str.find("vrc7") != std::string::npos)
                {
                    extdevice |= ExtDev::VRC7;
                }

                if (str.find("fds") != std::string::npos)
                {
                    extdevice |= ExtDev::FDS;
                }

                if (str.find("mmc5") != std::string::npos)
                {
                    extdevice |= ExtDev::MMC5;
                }

                if (str.find("n163") != std::string::npos)
                {
                    extdevice |= ExtDev::N163;
                }

                if (str.find("ss5b") != std::string::npos)
                {
                    extdevice |= ExtDev::SS5B;
                }
            }
        }
        else if (c == '@')
        {
            if (isNextStr("dpcm"))   //DPCMリスト
            {
                int tb;
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
                                            int size = Utils::GetFileSize(dpcm.path);
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
                            std::cerr << "Line " << linenum << " : Map diffinition #" << n << " is already exists." << std::endl;
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
                                            default:
                                                cmd = TONE;
                                                args.push_back(std::atoi(&c));
                                                break;
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
                                            cmd = VOLUME_ABS;
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
                                            if (getMultiHex(n))       //コマンド内容を保存
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
                                            std::cerr << "Line " << linenum << " : Invalid map difinition." << std::endl;
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
    int n;
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
                        int pos = ss.tellg();
                        std::vector<unsigned char> trhead;
						std::vector<unsigned char> trbody;
                        int tone = 0;
                        readBrackets(pos, 0, trhead, trbody, tone);
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
                                        ehead.push_back(ebody.size() / 2 + 1);  //ヘッダ自身を含めると+1

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
                                            ehead.push_back(ebody.size() / 2 + 1);
                                            ebody.push_back(0); //最後に0を追加にして次のエンベロープに流れないようにする
                                            ebody.push_back(0);
                                        }
                                        else if (ehead.size() == 1)
                                        {
                                            //ヘッダ1個だったらリリースが省略されたとみなす
                                            ehead[0] |= 0x80;
                                            ehead.push_back(ebody.size() / 2 + 1);
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

    for (const auto& [wavnum, data] : wavdata)
    {
        std::copy(data.begin(), data.end(), std::back_inserter(out));
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
        for (int i = 0; i < data.size(); i++)
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


void MMLReader::readBrackets(int startpos, int trheadsize, std::vector<unsigned char>& trhead, std::vector<unsigned char>& trbody, int& tone)
{
    char c;
    int n;
    int volume = 15;
    int octave = 4;
    int grace = 0;
    int loopmid_volume = 15;
	int loopmid_octave = 4;
    int loopmid_tone = 0;
    bool isTrack = false;
    bool isMusic = false;
    bool usePDelay = false;
    bool isLooped = false;
	bool isLoopedMid = false;
    int sweepStart = 0;
    int pddist = 4;
    int pdvol = 0;
    int pdlen = 0;
    int shorten = 0;
    int usingNoteMap = -1;
    std::map<Command, CommandArgs> usingCmds;
    std::vector<TrackData> tracks;
    TrackData tr;
    std::queue<unsigned char> prevnotes;
	std::vector<unsigned char> data;

    ss.seekg(startpos);

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
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
            nn = notetbl[c - tmp - 0x61] + octave * 12;
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
                            pushEnvAssign(data, envdata, cmd, args[0], args[1], true, offset);
                        }
                        else if (args.size() == 1)
                        {
                            pushEnvAssign(data, envdata, cmd, args[0], 0, true, offset);
                        }
                        else
                        {
                            std::cerr << "Line " << linenum << " : [Map difinition] Missing args." << std::endl;
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
                        data.push_back(cmd);

                        for (int i = 0; i < args.size(); i++)
                        {
                            if (i == 0)
                            {
                                sweepStart = i;
                            }
                            else if (i == 1)
                            {
                                data.push_back(args[i] - sweepStart);
                            }
                            else
                            {
                                data.push_back(args[i]);
                            }
                        }

                        usingCmds[cmd] = args;
                        if (usingCmds.count(cmd + 1))
                        {
                            usingCmds.erase(cmd + 1);   //停止コマンドがあったら消す
                        }
                    }
                    else if (cmd == PLAY_DATA)      //毎回実行するので使用中コマンドには追加しない
                    {
                        bool hasmusic = false;
                        for (int i = 0; i < musiclist.size(); i++)
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

                        for (int i = 0; i < args.size(); i++)
                        {
                            data.push_back(args[i]);
                        }

                        usingCmds[cmd] = args;

                        if (usingCmds.count(cmd - 1))
                        {
                            usingCmds.erase(cmd - 1);   //開始コマンドがあったら消す
                        }

                        if (cmd == TONE_ENV_STOP)
                        {
                            data.push_back(TONE);   //ドライバ側で戻す
                            data.push_back(tone);
                            usingCmds[TONE] = { tone };  //音色コマンドも使用中にする
                        }
                    }
                    else if (cmd == TONE)
                    {
                        data.push_back(TONE);

                        if (tr.device == DEV_2A03_DPCM)     //DPCMトラックなら
                        {
                            if (dpcmlist.count(args[0]))
                            {
                                data.push_back(dpcmlist[args[0]].offset / 0x40);
                                data.push_back(dpcmlist[args[0]].size / 0x10);
                                data.push_back(dpcmlist[args[0]].init);
                            }
                            else
                            {
                                std::cerr << "Line " << linenum << " : [Map difinition] DPCM #" << args[0] << " is not registered." << std::endl;
                                exit(1);
                            }
                        }
                        else
                        {
                            data.push_back(args[0]);
                            tone = args[0];
                        }
                        usingCmds[cmd] = args;
                    }
                    else if (cmd > 0x6b && cmd != REST_DEFLEN)  //ノートと休符以外
                    {
                        data.push_back(cmd);

                        for (int i = 0; i < args.size(); i++)
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
                    calcLength(REST_DEFLEN, data, lengthtbl, grace, 0);
                }
                else
                {
                    if (tr.device == DEV_2A03_NOISE || tr.device == DEV_2A03_DPCM)
                    {
                        //newNN = 0x0f - newNN & 0x0f;  //ドライバ側でやる
                        newNN = newNN & 0x0f;
                    }
                    else
                    {
                        newNN += sweepStart;
                    }

                    skipSpace();
                    calcLength(newNN, data, lengthtbl, grace, 0);
                }
            }
            else
            {
                if (tr.device == DEV_2A03_NOISE || tr.device == DEV_2A03_DPCM)
                {
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
                    while (prevnotes.size() > pddist)
                    {
                        prevnotes.pop();
                    }

                    calcLength(nn, data, lengthtbl, grace, pdlen);

                    if (pdvol < 0)
                    {
                        data.push_back(TAI_SLUR);
                    }
                    else
                    {
                        data.push_back(VOLUME_ABS);
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
                        data.push_back(VOLUME_ABS);
                        data.push_back(volume);
                    }
                }
                else
                {
                    calcLength(nn, data, lengthtbl, grace, 0);
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
                while (prevnotes.size() > pddist)
                {
                    prevnotes.pop();
                }

                calcLength(REST_DEFLEN, data, lengthtbl, grace, pdlen);

                if (pdvol > 0)
                {
                    data.push_back(VOLUME_ABS);
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
                    data.push_back(VOLUME_ABS);
                    data.push_back(volume);
                }
            }
            else
            {
                calcLength(REST_DEFLEN, data, lengthtbl, grace, 0);
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
                    data.push_back(TONE);
                    data.push_back(n);
					tone = n;   //音色を保存
                }
            }
            else if (isNextStr("fds"))
            {
                if ((extdevice & ExtDev::FDS) && tr.device == DEV_FDS)
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
                        for (int i = 0; i < musiclist.size(); i++)
                        {
                            if (musiclist[i] == n)
                            {
                                data.push_back(PLAY_DATA);
                                data.push_back(musiclist[i]);
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
                    getAndPushEnvAssign(data, envdata, FREQ_ENV, f_offset);
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
                    if (isNextChar('*'))
                    {
                        data.push_back(TONE_ENV_STOP);     //停止コマンド
                        data.push_back(TONE);
						data.push_back(tone);
                    }
                    else
                    {
                        if (getMultiDigit(n))
                        {
                            int envn = n + t_offset;
                            int delay = 0;
                            skipSpace();
                            if (isNextChar(','))
                            {
                                if (getMultiDigit(n))
                                {
                                    delay = n;
                                }
                            }

                            pushEnvAssign(data, envdata, TONE_ENV, envn, delay, false, 0);
                        }
                    }
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
                for (int i = 0; i < sizeof(nthnotetbl); i++)
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
                    data.push_back(VOLUME_REL);
                    data.push_back(n);
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
                    data.push_back(VOLUME_REL);
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
                    data.push_back(VOLUME_ABS);
                    data.push_back(n);
                    volume = n;
                }
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

                        data.push_back(TRACK_END);    //終了コード
                        tr.data = data;
                        tracks.push_back(tr);
                        data.clear();
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
                int tempo = 14400 / timebase;
                int fskip = 256 - 256 * n / tempo;
                int dir;

                if (fskip > 0)
                {
                    dir = 0;
                }
                else
                {
                    dir = 1;
                    fskip = -fskip;
                    //std::cerr << "Line " << linenum << " : Max tempo is " << tempo << std::endl;
                    //std::cerr << "Please reduce timebase." << std::endl;
                    //exit(1);
                }

                if (fskip > 255)
                {
                    fskip = 255;
                }

                data.push_back(TEMPO);
                data.push_back(dir);
                data.push_back(fskip);
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
                    data.push_back(SW_SWEEP_STOP);
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
                                    data.push_back(SW_SWEEP);
                                    data.push_back(end - start);
                                    data.push_back(delay);
                                    data.push_back(speed);
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
            if (getMultiHex(n))
            {
                int address, value;
                bool res = false;
                address = n;
                skipSpace();
                if (isNextChar(','))
                {
                    skipSpace();
                    if (getMultiHex(n))
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
                                    unsigned char v = 0x80 + (unsigned char)(rate & 0x07 << 4) + (unsigned char)(dir & 0x01 << 3) + ((unsigned char)amount & 0x07);
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
                                unsigned char v = (unsigned char)(loop & 0x01 << 5) + (unsigned char)(rate & 0x0f);
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
    int pos = 0;
    int posex = 0;

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
    int pos = 0;
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
    int pos = 0;
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


bool MMLReader::getMultiHex(int& res)
{
    char c;
    bool sign = true;
    std::string d;

    skipSpace();

    while (getc(c))
    {
        if (isHex(c))
        {
            d += c;
        }
        else if (c == '-')
        {
            if (d.empty())
            {
                d += c;
            }
            else
            {
                break;
            }
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
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


bool MMLReader::getMultiDigit(int& res)
{
    char c;
    bool sign = true;
    std::string d;

    skipSpace();

    while (getc(c))
    {
        if (isDigit(c))
        {
            d += c;
        }
        else if (c == '-')
        {
            if (d.empty())
            {
                d += c;
            }
            else
            {
                break;
            }
        }
        else
        {
            ss.seekg((int)ss.tellg() - 1);    //読み込み位置を戻す
            break;
        }
    }

    if (!d.empty())
    {
        res = std::stoi(d);
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


void MMLReader::calcLength(char cmd, std::vector<unsigned char>& data, std::vector<unsigned char> lengthtbl, int& prevGrace, int shorten)
{
    char c;
    int n = 0;
    int frames = 0;
    int dottedlen = 0;
    bool dotted = false;
    bool envdis = false;
    bool framelen = false;
    bool grace = false;
    std::string digit;

    while (getc(c))
    {
        if (c == '^' || (cmd == 0x6c && c == 'r'))      //タイ、もしくは休符が連続で来たとき
        {
            if (dotted)
            {
                frames += n;
            }
            else if (digit.empty())    //前に数値がない場合デフォ音長
            {
                frames += n;
                frames += timebase / deflen;
            }
            else
            {
                frames += n;
                frames += timebase / std::atoi(digit.c_str());
            }
            dottedlen = 0;
            dotted = false;
            digit.clear();
            n = 0;
        }
        else if (c == '.')   //付点
        {
            if (dotted)        //多重付点
            {
                dottedlen /= 2;
                n += dottedlen;
            }
            else if (digit.empty())    //前に数値がない場合デフォ音長
            {
                n = timebase / deflen;
                dottedlen = n / 2;
                n += dottedlen;
            }
            else
            {
                n = timebase / std::atoi(digit.c_str());
                dottedlen = n / 2;
                n += dottedlen;
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
                    n += timebase / std::atoi(digit.c_str());
                }
            }
            frames += n;
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
    int pos = ss.tellg();

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
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
        nn = notetbl[c - tmp - 0x61];
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
