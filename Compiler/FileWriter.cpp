#include "FileWriter.h"

static char neshead[]{
    0x4e, 0x45, 0x53, 0x1a, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static char nsfhead[]{
    0x4e, 0x45, 0x53, 0x4d, 0x1a, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x1a,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static char fdshead[]{
    0x46, 0x44, 0x53, 0x1a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


static char kyodaku[]{
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x17, 0x12, 0x17, 0x1d, 0x0e,
    0x17, 0x0d, 0x18, 0x24, 0x28, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x0f, 0x0a, 0x16, 0x12, 0x15, 0x22, 0x24, 0x0c, 0x18,
    0x16, 0x19, 0x1e, 0x1d, 0x0e, 0x1b, 0x24, 0x1d, 0x16, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24, 0x1d, 0x11, 0x12, 0x1c, 0x24, 0x19, 0x1b, 0x18, 0x0d, 0x1e, 0x0c, 0x1d, 0x24, 0x12,
    0x1c, 0x24, 0x16, 0x0a, 0x17, 0x1e, 0x0f, 0x0a, 0x0c, 0x1d, 0x1e, 0x1b, 0x0e, 0x0d, 0x24, 0x24,
    0x24, 0x24, 0x0a, 0x17, 0x0d, 0x24, 0x1c, 0x18, 0x15, 0x0d, 0x24, 0x0b, 0x22, 0x24, 0x17, 0x12,
    0x17, 0x1d, 0x0e, 0x17, 0x0d, 0x18, 0x24, 0x0c, 0x18, 0x27, 0x15, 0x1d, 0x0d, 0x26, 0x24, 0x24,
    0x24, 0x24, 0x18, 0x1b, 0x24, 0x0b, 0x22, 0x24, 0x18, 0x1d, 0x11, 0x0e, 0x1b, 0x24, 0x0c, 0x18,
    0x16, 0x19, 0x0a, 0x17, 0x22, 0x24, 0x1e, 0x17, 0x0d, 0x0e, 0x1b, 0x24, 0x24, 0x24, 0x24, 0x24,
    0x24, 0x24, 0x15, 0x12, 0x0c, 0x0e, 0x17, 0x1c, 0x0e, 0x24, 0x18, 0x0f, 0x24, 0x17, 0x12, 0x17,
    0x1d, 0x0e, 0x17, 0x0d, 0x18, 0x24, 0x0c, 0x18, 0x27, 0x15, 0x1d, 0x0d, 0x26, 0x26, 0x24, 0x24,
};

FileWriter::FileWriter()
{

}

FileWriter::FileWriter(std::wstring& path, MMLReader& reader)
{
    outputPath = path;
    title = reader.title;
    artist = reader.artist;
    copyright = reader.copyright;
    musicnum = reader.musiclist.size();
    dpcmoffset = reader.dpcmoffset;
    dpcmlist = reader.dpcmlist;
    seqdata = reader.seqdata;
    expdevice = reader.expdevice;
}

void FileWriter::createBin()
{
    std::ofstream ofs;
    ofs.open(outputPath, std::ofstream::out | std::ofstream::binary);
    if (!ofs)
    {
        std::cerr << "Faild to write file." << std::endl;
        exit(1);
    }

    for (auto it = seqdata.begin(); it != seqdata.end(); it++)
    {
        ofs.write((const char*)&*it, sizeof(char));
        if (!ofs)
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    ofs.close();
}

void FileWriter::createFds()
{
    std::wstring dir;
    Utils::GetModuleDir(dir);
    std::wstring drv = dir + L"bin\\dsp_fds_code.bin";
    std::wstring chr = dir + L"bin\\dsp_fds_chr.bin";
    std::wstring vec = dir + L"bin\\dsp_fds_vector.bin";

    const int fdssidesize = 65500;
    const int dpcmaddr = 0xc000;
    const int dpcmend = 0xdffa;
    const int prgaddr = 0x6000;
    const int chraddr = 0x0000;
    const int vecaddr = 0xdffa;

    auto readFile = [](const std::wstring& path)
    {
        std::vector<unsigned char> data;
        std::ifstream ifs;
        ifs.open(path, std::ifstream::in | std::ifstream::binary);
        if (!ifs)
        {
            std::cerr << "Faild to open dsp file." << std::endl;
            exit(1);
        }

        char c;
        while (true)
        {
            ifs.read(&c, sizeof(char));
            if (!ifs.eof())
            {
                data.push_back((unsigned char)c);
            }
            else
            {
                break;
            }
        }

        return data;
    };

    auto drvdata = readFile(drv);
    auto chrdata = readFile(chr);
    auto vecdata = readFile(vec);

    std::vector<unsigned char> dpcmdata;

    std::ifstream ifsd;
    int dpcmsize = 0;
    char c;

    for (const auto& [n, file] : dpcmlist)
    {
        dpcmsize += file.size;
    }

    if (dpcmaddr + dpcmsize + dpcmoffset > dpcmend)
    {
        std::cerr << "DPCM data size has reached maximum." << std::endl;
        std::cerr << "DPCM data : " << dpcmsize << " bytes, Max : " << dpcmend - dpcmoffset - dpcmaddr << " bytes" << std::endl;
        exit(1);
    }

    for (const auto& [n, file] : dpcmlist)
    {
        ifsd.open(std::filesystem::path(file.path), std::ifstream::binary);

        for (int i = 0; i < file.size; i++)
        {
            if (ifsd)
            {
                ifsd.read(&c, sizeof(char));
                dpcmdata.push_back((unsigned char)c);
            }
            else
            {
                std::cerr << "Faild to read DPCM file." << std::endl;
                exit(1);
            }
        }

        ifsd.close();
    }

    std::vector<unsigned char> prgdata;
    std::copy(drvdata.begin(), drvdata.end(), std::back_inserter(prgdata));
    std::copy(seqdata.begin(), seqdata.end(), std::back_inserter(prgdata));

    if (prgaddr + prgdata.size() > dpcmaddr + dpcmoffset)
    {
        std::cerr << "Sequence data size has reached maximum." << std::endl;
        std::cerr << "Seq data : " << seqdata.size() << " bytes, Max : " << dpcmaddr + dpcmoffset - (prgaddr + drvdata.size()) << " bytes" << std::endl;
        exit(1);
    }

    while (prgaddr + prgdata.size() < dpcmaddr + dpcmoffset)
    {
        prgdata.push_back(0);
    }

    std::copy(dpcmdata.begin(), dpcmdata.end(), std::back_inserter(prgdata));

    if (prgaddr + prgdata.size() > vecaddr)
    {
        std::cerr << "DPCM data size has reached maximum." << std::endl;
        std::cerr << "DPCM data : " << dpcmsize << " bytes, Max : " << vecaddr - dpcmoffset - dpcmaddr << " bytes" << std::endl;
        exit(1);
    }

    while (prgaddr + prgdata.size() < vecaddr)
    {
        prgdata.push_back(0);
    }

    std::copy(vecdata.begin(), vecdata.end(), std::back_inserter(prgdata));

    struct FdsFile
    {
        unsigned char number;
        unsigned char id;
        const char* name;
        int address;
        unsigned char type;
        std::vector<unsigned char> data;
    };

    std::vector<unsigned char> hackdata(0xe0, 0);
    const unsigned char nmihack[]{
        0x48, 0xa9, 0x00, 0x8d, 0x00, 0x20, 0xa9, 0x0f,
        0x8d, 0xfe, 0x01, 0xa9, 0x01, 0x8d, 0xff, 0x01,
        0xa9, 0x6c, 0x8d, 0x10, 0x01, 0xa9, 0xfc, 0x8d,
        0x11, 0x01, 0xa9, 0xdf, 0x8d, 0x12, 0x01, 0xa9,
        0xef, 0x8d, 0xfa, 0xdf, 0xa9, 0x35, 0x8d, 0x02,
        0x01, 0xa9, 0xac, 0x8d, 0x04, 0x01, 0x68, 0x40
    };
    std::copy(std::begin(nmihack), std::end(nmihack), hackdata.begin() + 0xa0);
    hackdata[0xda] = 0xc0;
    hackdata[0xdb] = 0xdf;

    std::vector<unsigned char> gotonmi(0x100, 0);
    for (int i = 8; i <= 0xf0; i += 8)
    {
        gotonmi[i] = 0x90;
    }

    std::vector<FdsFile> files;
    files.push_back({ 0, 0, "HACK-PRG", 0xdf20, 0, hackdata });
    files.push_back({ 1, 0, "GOTO-NMI", 0x2000, 0, gotonmi });
    files.push_back({ 2, 0, "FCDSPPRG", prgaddr, 0, prgdata });
    files.push_back({ 3, 0, "FCDSPCHR", chraddr, 1, chrdata });
    const unsigned char lastFileId = files.back().id;
    const unsigned char visibleFileCount = (unsigned char)files.size();

    std::vector<unsigned char> side;
    auto pushByte = [&side](unsigned char value)
    {
        side.push_back(value);
    };
    auto pushWord = [&side](int value)
    {
        side.push_back((unsigned char)(value & 0xff));
        side.push_back((unsigned char)((value >> 8) & 0xff));
    };
    auto pushString = [&side](const char* text, int length)
    {
        for (int i = 0; i < length; i++)
        {
            side.push_back((unsigned char)text[i]);
        }
    };

    pushByte(0x01);
    pushString("*NINTENDO-HVC*", 14);
    pushByte(0x01);
    pushString("FDS", 3);
    pushByte(0x20);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x00);
	pushByte(lastFileId);   // 最初に読み込むファイル（すべてのファイルを読み込む）
    for (int i = 0; i < 5; i++)
    {
        pushByte(0xff);
    }
    pushByte(0x61);     // 製造年月日
    pushByte(0x01);
    pushByte(0x01);
    pushByte(0x49);
    pushByte(0x61);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x02);
    for (int i = 0; i < 5; i++)
    {
        pushByte(0x00);
    }
    pushByte(0x61);     // 書き換え年月日
    pushByte(0x01);
    pushByte(0x01);
    pushByte(0x00);
    pushByte(0x80);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x07);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x00);
    pushByte(0x00);

    pushByte(0x02);
	pushByte(visibleFileCount);

    for (const auto& file : files)
    {
        pushByte(0x03);
        pushByte(file.number);
        pushByte(file.id);
        pushString(file.name, 8);
        pushWord(file.address);
        pushWord((int)file.data.size());
        pushByte(file.type);

        pushByte(0x04);
        std::copy(file.data.begin(), file.data.end(), std::back_inserter(side));
    }

    if (side.size() > fdssidesize)
    {
        std::cerr << "FDS disk side size has reached maximum." << std::endl;
        std::cerr << "FDS side : " << side.size() << " bytes, Max : " << fdssidesize << " bytes" << std::endl;
        exit(1);
    }

    while (side.size() < fdssidesize)
    {
        side.push_back(0);
    }

    std::ofstream ofs;
    ofs.open(outputPath, std::ofstream::out | std::ofstream::binary);
    if (!ofs)
    {
        std::cerr << "Faild to write file." << std::endl;
        exit(1);
    }

    for (const auto& h : fdshead)
    {
        ofs.write(&h, sizeof(char));
    }

    for (const auto& s : side)
    {
        ofs.write((const char*)&s, sizeof(char));
    }

    ofs.close();
}


void FileWriter::createNes()
{
    std::ifstream ifs;
    std::wstring dir;
    Utils::GetModuleDir(dir);
    std::wstring drv = dir;
    std::wstring data = dir;

    if (expdevice & Expdev::VRC6)
    {
        drv += L"bin\\dsp_vrc6_code.bin";
        data += L"bin\\dsp_vrc6_data.bin";
    }
    else if (expdevice & Expdev::VRC7)
    {
        std::cerr << "VRC7 is not supported by NES file output." << std::endl;
        exit(1);
    }
    else if (expdevice & Expdev::FDS)
    {
        std::cerr << "FDS is not supported by NES file output." << std::endl;
        exit(1);
    }
    else if (expdevice & Expdev::MMC5)
    {
        drv += L"bin\\dsp_mmc5_code.bin";
        data += L"bin\\dsp_mmc5_data.bin";
    }
    else if (expdevice & Expdev::N163)
    {
        drv += L"bin\\dsp_n163_code.bin";
        data += L"bin\\dsp_n163_data.bin";
    }
    else if (expdevice & Expdev::SS5B)
    {
        drv += L"bin\\dsp_ss5b_code.bin";
        data += L"bin\\dsp_ss5b_data.bin";
    }
    else
    {
        //2A03 with MMC3 banking
        drv += L"bin\\dsp_mmc3_code.bin";
		data += L"bin\\dsp_mmc3_data.bin";
    }


    auto drvsize = Utils::GetFileSize(drv);

    ifs.open(drv, std::ifstream::in | std::ifstream::binary);
    if (!ifs)
    {
        std::cerr << "Faild to open dsp code." << std::endl;
        exit(1);
    }

    std::ofstream ofs;
    ofs.open(outputPath, std::ofstream::out | std::ofstream::binary);
    if (!ofs)
    {
        std::cerr << "Faild to write file." << std::endl;
        exit(1);
    }

    char c;
    int nesheadsize = 0x10;
    size_t dpcmaddr = 0x4000 + nesheadsize;
    size_t dpcmend = 0x7eb0 + nesheadsize;

    if (expdevice & Expdev::VRC6)
    {
        neshead[0x04] = 0x02; //PRG16K x2
        neshead[0x06] = 0x81; //VRC6
		neshead[0x07] = 0x10; //VRC6
	}
    else if (expdevice & Expdev::MMC5)
    {
        neshead[0x04] = 0x02; //PRG16K x2
        neshead[0x06] = 0x51; //MMC5
		neshead[0x07] = 0x0;  //MMC5
    }
    else if (expdevice & Expdev::SS5B)
    {
        neshead[0x04] = 0x02; //PRG16K x2
        neshead[0x06] = 0x51; //Sunsoft FME-7 / 5B
		neshead[0x07] = 0x40;
    }
    else if (expdevice & Expdev::N163)
    {
        neshead[0x04] = 0x02; //PRG16K x2
        neshead[0x06] = 0x31; //N163
		neshead[0x07] = 0x10;
    }
    else
    {
        neshead[0x04] = 0x02;
        neshead[0x06] = 0x41; //MMC3, vertical mirroring
        neshead[0x07] = 0x00;
    }

    const bool mmc3Banked = expdevice == 0;
    const bool bankedDsp = mmc3Banked || (expdevice & (Expdev::VRC6 | Expdev::MMC5 | Expdev::SS5B | Expdev::N163));
    if (bankedDsp)
    {
        const int musicbanksize = 0x2000;
        const int fixedprgsize = 0x2000;
        const bool vrc6Banked = expdevice & Expdev::VRC6;
        const bool n163Banked = expdevice & Expdev::N163;
        const bool fullDpcmRange = mmc3Banked || (expdevice & (Expdev::MMC5 | Expdev::SS5B));
        const int fixedDataSize = n163Banked ? fixedprgsize : (mmc3Banked ? 0x15c : 0x150);
        const int firstMusicBank = vrc6Banked ? 0 : 1;
        const int fixedcodesize = vrc6Banked ? 0 : musicbanksize;

        if (drvsize > musicbanksize)
        {
            std::cerr << "DSP driver size has reached banked-code maximum." << std::endl;
            std::cerr << "Driver data : " << drvsize << " bytes, Max : " << fixedcodesize << " bytes" << std::endl;
            exit(1);
        }

        auto readWord = [](const std::vector<unsigned char>& bytes, int pos)
        {
            return static_cast<int>(bytes[pos]) | (static_cast<int>(bytes[pos + 1]) << 8);
        };
        auto writeWord = [](std::vector<unsigned char>& bytes, int pos, int value)
        {
            bytes[pos] = value & 0xff;
            bytes[pos + 1] = (value >> 8) & 0xff;
        };

        if (musicnum < 1 || seqdata.size() < static_cast<size_t>(musicnum * 2))
        {
            std::cerr << "Invalid sequence data." << std::endl;
            exit(1);
        }

        std::vector<int> musicaddr;
        for (int i = 0; i < musicnum; i++)
        {
            musicaddr.push_back(readWord(seqdata, i * 2));
        }
        int commonend = static_cast<int>(seqdata.size());
        for (const auto& addr : musicaddr)
        {
            if (addr < commonend)
            {
                commonend = addr;
            }
        }

        struct MusicBanks
        {
            std::vector<unsigned char> first;
            std::vector<std::vector<unsigned char>> additional;
        };
        int nextAdditionalBank = firstMusicBank + musicnum;

        auto makeMusicBanks = [&](int music) -> MusicBanks
        {
            int oldstart = musicaddr[music];
            int oldend = static_cast<int>(seqdata.size());
            for (const auto& addr : musicaddr)
            {
                if (addr > oldstart && addr < oldend)
                {
                    oldend = addr;
                }
            }
            if (oldstart < commonend || oldend > static_cast<int>(seqdata.size()))
            {
                std::cerr << "Invalid sequence address." << std::endl;
                exit(1);
            }

            int pos = oldstart;
            std::vector<int> trackaddr;
            while (pos < oldend && seqdata[pos] != 0xff)
            {
                if (pos + 3 >= oldend)
                {
                    std::cerr << "Invalid track header." << std::endl;
                    exit(1);
                }
                trackaddr.push_back(readWord(seqdata, pos + 2));
                pos += 4;
            }
            if (trackaddr.empty() || pos >= oldend)
            {
                std::cerr << "Invalid track header." << std::endl;
                exit(1);
            }

            int oldTrackDataStart = *std::min_element(trackaddr.begin(), trackaddr.end());
            if (oldTrackDataStart <= pos || oldTrackDataStart > oldend)
            {
                std::cerr << "Invalid track address." << std::endl;
                exit(1);
            }

            std::vector<int> trackbank{ firstMusicBank + music };
            for (size_t i = 1; i < trackaddr.size(); i++)
            {
                trackbank.push_back(nextAdditionalBank++);
            }

            const int newstart = commonend;
            const int newTrackDataStart = newstart + (oldTrackDataStart - oldstart) + static_cast<int>(trackaddr.size());
            MusicBanks result;
            for (size_t track = 0; track < trackaddr.size(); track++)
            {
                int trackend = oldend;
                for (const auto& addr : trackaddr)
                {
                    if (addr > trackaddr[track] && addr < trackend)
                    {
                        trackend = addr;
                    }
                }

                std::vector<unsigned char> bank(seqdata.begin(), seqdata.begin() + commonend);
                for (int i = 0; i < musicnum; i++)
                {
                    writeWord(bank, i * 2, newstart);
                }
                int headerpos = oldstart;
                while (seqdata[headerpos] != 0xff)
                {
                    bank.push_back(seqdata[headerpos]);
                    bank.push_back(seqdata[headerpos + 1]);
                    bank.push_back(newTrackDataStart & 0xff);
                    bank.push_back((newTrackDataStart >> 8) & 0xff);
                    bank.push_back(trackbank[(headerpos - oldstart) / 4]);
                    headerpos += 4;
                }
                std::copy(seqdata.begin() + headerpos, seqdata.begin() + oldTrackDataStart, std::back_inserter(bank));
                std::copy(seqdata.begin() + trackaddr[track], seqdata.begin() + trackend, std::back_inserter(bank));
                if (bank.size() > musicbanksize)
                {
                    std::cerr << "Sequence data size has reached maximum for a track." << std::endl;
                    std::cerr << "Track data : " << bank.size() << " bytes, Max : " << musicbanksize << " bytes" << std::endl;
                    exit(1);
                }
                bank.resize(musicbanksize, 0);
                if (track == 0)
                {
                    result.first = std::move(bank);
                }
                else
                {
                    result.additional.push_back(std::move(bank));
                }
            }
            return result;
        };

        std::vector<MusicBanks> musicbanks;
        for (int i = 0; i < musicnum; i++)
        {
            musicbanks.push_back(makeMusicBanks(i));
        }

        auto loadDpcmData = [&](int capacity)
        {
            std::vector<unsigned char> dpcmdata(capacity, 0);
            int dpcmpos = dpcmoffset;
            for (const auto& [n, file] : dpcmlist)
            {
                if (dpcmpos + file.size > capacity)
                {
                    std::cerr << "DPCM data size has reached maximum." << std::endl;
                    exit(1);
                }
                std::ifstream dpcmifs(std::filesystem::path(file.path), std::ifstream::binary);
                dpcmifs.read(reinterpret_cast<char*>(dpcmdata.data() + dpcmpos), file.size);
                if (!dpcmifs)
                {
                    std::cerr << "Faild to read DPCM file." << std::endl;
                    exit(1);
                }
                dpcmpos += file.size;
            }
            return dpcmdata;
        };

        auto writeBank = [&](const std::vector<unsigned char>& bank)
        {
            ofs.write(reinterpret_cast<const char*>(bank.data()), bank.size());
        };

        if (vrc6Banked)
        {
            const int musicSlotCount = 15;
            if (nextAdditionalBank > musicSlotCount)
            {
                std::cerr << "DSP PRG bank count has reached mapper maximum." << std::endl;
                exit(1);
            }
            neshead[0x04] = 0x10;
            for (const auto& h : neshead)
            {
                ofs.write(&h, sizeof(char));
            }
            std::vector<unsigned char> padding(musicbanksize, 0);
            int writtenSlots = 0;
            auto writeVrc6Music = [&](const std::vector<unsigned char>& bank)
            {
                writeBank(bank);
                writeBank(padding);
                writtenSlots++;
            };
            for (const auto& banks : musicbanks)
            {
                writeVrc6Music(banks.first);
            }
            for (const auto& banks : musicbanks)
            {
                for (const auto& bank : banks.additional)
                {
                    writeVrc6Music(bank);
                }
            }
            while (writtenSlots < musicSlotCount)
            {
                writeBank(padding);
                writeBank(padding);
                writtenSlots++;
            }
            auto dpcmdata = loadDpcmData(musicbanksize);
            writeBank(dpcmdata);

            std::vector<unsigned char> fixedBank(fixedprgsize, 0);
            ifs.read(reinterpret_cast<char*>(fixedBank.data()), fixedBank.size());
            if (ifs.gcount() != fixedBank.size())
            {
                std::cerr << "Invalid DSP fixed driver." << std::endl;
                exit(1);
            }
            writeBank(fixedBank);

            std::ifstream ifsc(data, std::ifstream::in | std::ifstream::binary);
            while (ifsc.read(&c, sizeof(char)))
            {
                ofs.write(&c, sizeof(char));
            }
            if (!ofs)
            {
                std::cerr << "Faild to write file." << std::endl;
                exit(1);
            }
            ofs.close();
            return;
        }

        int highestBank = nextAdditionalBank + 1;
        if ((highestBank & 1) == 0)
        {
            highestBank++;
        }
        int bankLimit = (expdevice & Expdev::VRC6) ? 0x1f :
            (expdevice & Expdev::MMC5) ? 0x7f : 0x3f;
        if (highestBank > bankLimit)
        {
            std::cerr << "DSP PRG bank count has reached mapper maximum." << std::endl;
            exit(1);
        }

        neshead[0x04] = (highestBank + 1) / 2;
        for (const auto& h : neshead)
        {
            ofs.write(&h, sizeof(char));
        }

        for (int i = 0; i < fixedcodesize; i++)
        {
            ifs.read(&c, sizeof(char));
            if (ifs)
            {
                ofs.write(&c, sizeof(char));
            }
            else
            {
                c = 0;
                ofs.write(&c, sizeof(char));
            }
        }
        for (const auto& banks : musicbanks)
        {
            writeBank(banks.first);
        }
        for (const auto& banks : musicbanks)
        {
            for (const auto& bank : banks.additional)
            {
                writeBank(bank);
            }
        }
        int writtenBanks = nextAdditionalBank;
        int paddingEnd = highestBank - 1;
        while (writtenBanks < paddingEnd)
        {
            std::vector<unsigned char> padding(musicbanksize, 0);
            writeBank(padding);
            writtenBanks++;
        }

        int dpcmCapacity = mmc3Banked ? 0x3ea4 : (fullDpcmRange ? 0x3eb0 : musicbanksize);
        auto dpcmdata = loadDpcmData(dpcmCapacity);
        std::vector<unsigned char> dpcmLower(dpcmdata.begin(), dpcmdata.begin() + musicbanksize);
        writeBank(dpcmLower);

        std::vector<unsigned char> fixedData;
        std::ifstream ifsc(data, std::ifstream::in | std::ifstream::binary);
        while (ifsc.read(&c, sizeof(char)))
        {
            fixedData.push_back(static_cast<unsigned char>(c));
        }
        if (fixedData.size() < fixedDataSize)
        {
            std::cerr << "Invalid DSP fixed data." << std::endl;
            exit(1);
        }
        std::vector<unsigned char> fixedBank(fixedprgsize, 0);
        if (fullDpcmRange)
        {
            std::copy(dpcmdata.begin() + musicbanksize, dpcmdata.end(), fixedBank.begin());
        }
        std::copy(fixedData.begin(), fixedData.begin() + fixedDataSize,
            fixedBank.begin() + fixedprgsize - fixedDataSize);
        writeBank(fixedBank);
        ofs.write(reinterpret_cast<const char*>(fixedData.data() + fixedDataSize), fixedData.size() - fixedDataSize);
        if (!ofs)
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
        ofs.close();
        return;
    }

    if (nesheadsize + drvsize + seqdata.size() > dpcmaddr + dpcmoffset)
    {
        std::cerr << "Sequence data size has reached maximum." << std::endl;
        std::cerr << "Seq data : " << seqdata.size() << " bytes, Max : " << dpcmaddr + dpcmoffset - (nesheadsize + drvsize) << " bytes" << std::endl;
        exit(1);
    }

    for (const auto& h : neshead)
    {
        if (ofs)
        {
            ofs.write(&h, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < drvsize; i++)
    {
        if (ifs && ofs)
        {
            ifs.read(&c, sizeof(char));
            ofs.write(&c, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    ifs.close();

    for (const auto& s : seqdata)
    {
        if (ofs)
        {
            ofs.write((const char*)&s, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    c = 0;
    for (size_t i = drvsize + nesheadsize + seqdata.size(); i < dpcmaddr + dpcmoffset; i++)
    {
        if (ofs)
        {
            ofs.write(&c, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    int dpcmsize = 0;
    std::ifstream ifsd;

    for (const auto& [n, file] : dpcmlist)
    {
        dpcmsize += file.size;
    }

    if (dpcmaddr + dpcmsize + dpcmoffset > dpcmend)
    {
        std::cerr << "DPCM data size has reached maximum." << std::endl;
        std::cerr << "DPCM data : " << dpcmsize << " bytes, Max : " << dpcmend - dpcmoffset - dpcmaddr << " bytes" << std::endl;
        exit(1);
    }

    for (const auto& [n, file] : dpcmlist)
    {
        ifsd.open(std::filesystem::path(file.path), std::ifstream::binary);

        for (int i = 0; i < file.size; i++)
        {
            if (ifsd && ofs)
            {
                ifsd.read(&c, sizeof(char));
                ofs.write(&c, sizeof(char));
            }
            else
            {
                std::cerr << "Faild to write file." << std::endl;
                exit(1);
            }
        }

        ifsd.close();
    }

    c = 0;
    for (size_t i = dpcmaddr + dpcmsize; i < dpcmend; i++)
    {
        if (ofs)
        {
            ofs.write(&c, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    std::ifstream ifsc;
    ifsc.open(data, std::ifstream::in | std::ifstream::binary);
    if (!ifs)
    {
        std::cerr << "Faild to open dsp data." << std::endl;
        exit(1);
    }

    while (true)
    {
        if (ifsc && ofs)
        {
            ifsc.read(&c, sizeof(char));
            if (!ifsc.eof())
            {
                ofs.write(&c, sizeof(char));
            }
            else
            {
                break;
            }
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    ifsc.close();
    ofs.close();
}


void FileWriter::createNsf()
{
    std::ifstream ifs;
    std::wstring dir;
    Utils::GetModuleDir(dir);
    std::wstring drv = dir;
    const int fixeddrvsize = 0x2000;
    const int musicbanksize = 0x2000;
    const int dpcmbanksize = 0x4000;

    char c;

    nsfhead[0x06] = musicnum; // 曲数
    nsfhead[0x08] = 0x00;     // シーケンスデータの開始アドレス
    nsfhead[0x09] = 0x80;
    nsfhead[0x7b] = expdevice;   // 拡張音源

    if (expdevice & Expdev::VRC6)
    {
        drv += L"bin\\drv_vrc6.bin";
    }
    else if (expdevice & Expdev::VRC7)
    {
        drv += L"bin\\drv_vrc7.bin";
    }
    else if (expdevice & Expdev::FDS)
    {
        drv += L"bin\\drv_fds.bin";
    }
    else if (expdevice & Expdev::MMC5)
    {
        drv += L"bin\\drv_mmc5.bin";
    }
    else if (expdevice & Expdev::N163)
    {
        drv += L"bin\\drv_n163.bin";
    }
    else if (expdevice & Expdev::SS5B)
    {
        drv += L"bin\\drv_ss5b.bin";
    }
    else
    {
        //2A03
        drv += L"bin\\drv.bin";
    }

    auto drvsize = Utils::GetFileSize(drv);
    if (drvsize > fixeddrvsize)
    {
        std::cerr << "NSF driver size has reached maximum." << std::endl;
        std::cerr << "Driver data : " << drvsize << " bytes, Max : " << fixeddrvsize << " bytes" << std::endl;
        exit(1);
    }

    ifs.open(drv, std::ifstream::in | std::ifstream::binary);
    if (!ifs)
    {
        std::cerr << "Faild file open driver file." << std::endl;
        exit(1);
    }

    unsigned char driverMetadata[11];
    ifs.read(reinterpret_cast<char*>(driverMetadata), sizeof(driverMetadata));
    if (ifs.gcount() != sizeof(driverMetadata) ||
        driverMetadata[0] != 'D' || driverMetadata[1] != 'R' ||
        driverMetadata[2] != 'F' || driverMetadata[3] != 'M' ||
        driverMetadata[4] != 'N' || driverMetadata[5] != 'S' ||
        driverMetadata[6] != 'F')
    {
        std::cerr << "Invalid driver metadata." << std::endl;
        exit(1);
    }

    nsfhead[0x0a] = driverMetadata[7];   // Init address
    nsfhead[0x0b] = driverMetadata[8];
    nsfhead[0x0c] = driverMetadata[9];   // Play address
    nsfhead[0x0d] = driverMetadata[10];

    for (int i = 0; i < 8; i++)
    {
        nsfhead[0x70 + i] = i;
    }

    ifs.clear();
    ifs.seekg(0, std::ios::beg);

    std::ofstream ofs;
    ofs.open(outputPath, std::ofstream::out | std::ofstream::binary);
    if (!ofs)
    {
        std::cerr << "Faild to write file." << std::endl;
        exit(1);
    }

    for (int i = 0; i < 31; i++)
    {
        if (i < (int)title.length())
        {
            nsfhead[i + 0x0e] = title[i];
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < 31; i++)
    {
        if (i < (int)artist.length())
        {
            nsfhead[i + 0x2e] = artist[i];
        }
        else
        {
            break;
        }
    }

    for (int i = 0; i < 31; i++)
    {
        if (i < (int)copyright.length())
        {
            nsfhead[i + 0x4e] = copyright[i];
        }
        else
        {
            break;
        }
    }

    for (const auto& h : nsfhead)
    {
        if (ofs)
        {
            ofs.write(&h, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    for (int i = 0; i < fixeddrvsize; i++)
    {
        if (ofs)
        {
            ifs.read(&c, sizeof(char));
            if (!ifs.eof())
            {
                ofs.write(&c, sizeof(char));
            }
            else
            {
                c = 0;
                ofs.write(&c, sizeof(char));
            }
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    auto readWord = [](const std::vector<unsigned char>& data, int pos)
    {
        return static_cast<int>(data[pos]) | (static_cast<int>(data[pos + 1]) << 8);
    };

    auto writeWord = [](std::vector<unsigned char>& data, int pos, int value)
    {
        data[pos] = value & 0xff;
        data[pos + 1] = (value >> 8) & 0xff;
    };

    if (musicnum < 1 || seqdata.size() < static_cast<size_t>(musicnum * 2))
    {
        std::cerr << "Invalid sequence data." << std::endl;
        exit(1);
    }

    std::vector<int> musicaddr;
    for (int i = 0; i < musicnum; i++)
    {
        musicaddr.push_back(readWord(seqdata, i * 2));
    }

    int commonend = static_cast<int>(seqdata.size());
    for (const auto& addr : musicaddr)
    {
        if (addr < commonend)
        {
            commonend = addr;
        }
    }

    if (commonend < musicnum * 2 || commonend > static_cast<int>(seqdata.size()))
    {
        std::cerr << "Invalid sequence address." << std::endl;
        exit(1);
    }

    struct MusicBanks
    {
        std::vector<unsigned char> first;
        std::vector<std::vector<unsigned char>> additional;
    };

    int nextAdditionalBank = 2 + musicnum * 2;

    auto makeMusicBanks = [&](int music) -> MusicBanks
    {
        int oldstart = musicaddr[music];
        int oldend = static_cast<int>(seqdata.size());
        for (const auto& addr : musicaddr)
        {
            if (addr > oldstart && addr < oldend)
            {
                oldend = addr;
            }
        }

        if (oldstart < commonend || oldend > static_cast<int>(seqdata.size()))
        {
            std::cerr << "Invalid sequence address." << std::endl;
            exit(1);
        }

        int pos = oldstart;
        std::vector<int> trackaddr;
        while (pos < oldend && seqdata[pos] != 0xff)
        {
            if (pos + 3 >= oldend)
            {
                std::cerr << "Invalid track header." << std::endl;
                exit(1);
            }
            trackaddr.push_back(readWord(seqdata, pos + 2));
            pos += 4;
        }

        if (trackaddr.empty() || pos >= oldend)
        {
            std::cerr << "Invalid track header." << std::endl;
            exit(1);
        }

        int oldTrackDataStart = *std::min_element(trackaddr.begin(), trackaddr.end());
        if (oldTrackDataStart <= pos || oldTrackDataStart > oldend)
        {
            std::cerr << "Invalid track address." << std::endl;
            exit(1);
        }

        std::vector<int> trackbank;
        trackbank.push_back(2 + music * 2);
        for (size_t i = 1; i < trackaddr.size(); i++)
        {
            trackbank.push_back(nextAdditionalBank);
            nextAdditionalBank += 2;
        }

        const int newstart = commonend;
        const int newTrackDataStart = newstart + (oldTrackDataStart - oldstart) + static_cast<int>(trackaddr.size());
        MusicBanks result;

        for (size_t track = 0; track < trackaddr.size(); track++)
        {
            int trackend = oldend;
            for (const auto& addr : trackaddr)
            {
                if (addr > trackaddr[track] && addr < trackend)
                {
                    trackend = addr;
                }
            }

            std::vector<unsigned char> bank(seqdata.begin(), seqdata.begin() + commonend);
            for (int i = 0; i < musicnum; i++)
            {
                writeWord(bank, i * 2, newstart);
            }

            int headerpos = oldstart;
            while (seqdata[headerpos] != 0xff)
            {
                bank.push_back(seqdata[headerpos]);
                bank.push_back(seqdata[headerpos + 1]);
                bank.push_back(newTrackDataStart & 0xff);
                bank.push_back((newTrackDataStart >> 8) & 0xff);
                bank.push_back(trackbank[(headerpos - oldstart) / 4]);
                headerpos += 4;
            }
            std::copy(seqdata.begin() + headerpos, seqdata.begin() + oldTrackDataStart, std::back_inserter(bank));
            std::copy(seqdata.begin() + trackaddr[track], seqdata.begin() + trackend, std::back_inserter(bank));

            if (bank.size() > musicbanksize)
            {
                std::cerr << "Sequence data size has reached maximum for a track." << std::endl;
                std::cerr << "Track data : " << bank.size() << " bytes, Max : " << musicbanksize << " bytes" << std::endl;
                exit(1);
            }

            bank.resize(musicbanksize, 0);
            if (track == 0)
            {
                result.first = std::move(bank);
            }
            else
            {
                result.additional.push_back(std::move(bank));
            }
        }
        return result;
    };

    std::vector<MusicBanks> musicbanks;
    for (int i = 0; i < musicnum; i++)
    {
        musicbanks.push_back(makeMusicBanks(i));
    }

    int dpcmbank = nextAdditionalBank;
    if (dpcmbank + 3 > 0xff)
    {
        std::cerr << "NSF bank count has reached maximum." << std::endl;
        exit(1);
    }
    nsfhead[0x74] = dpcmbank;
    nsfhead[0x75] = dpcmbank + 1;
    nsfhead[0x76] = dpcmbank + 2;
    nsfhead[0x77] = dpcmbank + 3;

    auto musicDataPos = ofs.tellp();
    ofs.seekp(0x74, std::ios::beg);
    ofs.write(&nsfhead[0x74], 4);
    ofs.seekp(musicDataPos, std::ios::beg);
    if (!ofs)
    {
        std::cerr << "Faild to update NSF bank header." << std::endl;
        exit(1);
    }

    auto writeBank = [&](const std::vector<unsigned char>& bank)
    {
        for (const auto& s : bank)
        {
            if (ofs)
            {
                ofs.write(reinterpret_cast<const char*>(&s), sizeof(char));
            }
            else
            {
                std::cerr << "Faild to write file." << std::endl;
                exit(1);
            }
        }
    };

    for (const auto& banks : musicbanks)
    {
        writeBank(banks.first);
    }
    for (const auto& banks : musicbanks)
    {
        for (const auto& bank : banks.additional)
        {
            writeBank(bank);
        }
    }

    std::ifstream ifsd;
    int dpcmsize = 0;

    for (const auto& [n, file] : dpcmlist)
    {
        dpcmsize += file.size;
    }

    if (dpcmsize + dpcmoffset > dpcmbanksize)
    {
        std::cerr << "DPCM data size has reached maximum." << std::endl;
        std::cerr << "DPCM data : " << dpcmsize << " bytes, Max : " << dpcmbanksize - dpcmoffset << " bytes" << std::endl;
        exit(1);
    }

    for (int i = 0; i < dpcmoffset; i++)
    {
        if (ofs)
        {
            c = 0;
            ofs.write(&c, sizeof(char));
        }
        else
        {
            std::cerr << "Faild to write file." << std::endl;
            exit(1);
        }
    }

    for (const auto& [n, file] : dpcmlist)
    {
        ifsd.open(std::filesystem::path(file.path), std::ifstream::binary);

        for (int i = 0; i < file.size; i++)
        {
            if (ifsd && ofs)
            {
                ifsd.read(&c, sizeof(char));
                ofs.write(&c, sizeof(char));
            }
            else
            {
                std::cerr << "Faild to write file." << std::endl;
                exit(1);
            }
        }

        ifsd.close();
    }

    ofs.close();
}
