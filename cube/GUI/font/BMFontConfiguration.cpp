#include "BMFontConfiguration.h"

#include <map>
#include "external/TUtility/Data.h"
#include "external/TUtility/file/Tfile.h"
using namespace std;
//
//FNTConfig Cache - free functions
//
static map<std::string, BMFontConfiguration*>* s_configurations = nullptr;

BMFontConfiguration* FNTConfigLoadFile(const std::string& fntFile)
{
    BMFontConfiguration* ret = nullptr;

    if( s_configurations == nullptr )
    {
        s_configurations = new (std::nothrow) map<std::string, BMFontConfiguration*>();
    }

    ret = s_configurations->at(fntFile);
    if( ret == nullptr )
    {
        ret = BMFontConfiguration::create(fntFile.c_str());
        if (ret)
        {
            s_configurations->insert(std::make_pair (fntFile, ret));
        }
    }

    return ret;
}

//
//BitmapFontConfiguration
//

BMFontConfiguration * BMFontConfiguration::create(const std::string& FNTfile)
{
    BMFontConfiguration * ret = new (std::nothrow) BMFontConfiguration();
    if (ret->initWithFNTfile(FNTfile))
    {
        return ret;
    }
    delete ret;
    return nullptr;
}

bool BMFontConfiguration::initWithFNTfile(const std::string& FNTfile)
{
    _kerningDictionary = nullptr;
    _fontDefDictionary = nullptr;

    _characterSet = this->parseConfigFile(FNTfile);

    if (! _characterSet)
    {
        return false;
    }

    return true;
}

std::set<unsigned int>* BMFontConfiguration::getCharacterSet() const
{
    return _characterSet;
}

_BMFontDef BMFontConfiguration::getFontDef(int c)
{
    _FontDefHashElement *element = nullptr;
    HASH_FIND_INT(_fontDefDictionary, &c, element);
    return element->fontDef;
}

BMFontConfiguration::BMFontConfiguration()
: _fontDefDictionary(nullptr)
, _commonHeight(0)
, _kerningDictionary(nullptr)
, _characterSet(nullptr)
{

}

BMFontConfiguration::~BMFontConfiguration()
{
    this->purgeFontDefDictionary();
    this->purgeKerningDictionary();
    _atlasName.clear();
    delete _characterSet;
}

std::string BMFontConfiguration::description(void) const
{
}

void BMFontConfiguration::purgeKerningDictionary()
{
    tKerningHashElement *current;
    while(_kerningDictionary)
    {
        current = _kerningDictionary;
        HASH_DEL(_kerningDictionary,current);
        free(current);
    }
}

void BMFontConfiguration::purgeFontDefDictionary()
{
    tFontDefHashElement *current, *tmp;

    HASH_ITER(hh, _fontDefDictionary, current, tmp) {
        HASH_DEL(_fontDefDictionary, current);
        free(current);
    }
}
int BMFontConfiguration::fontSize() const
{
    return m_fontSize;
}

void BMFontConfiguration::setFontSize(int fontSize)
{
    m_fontSize = fontSize;
}


std::set<unsigned int>* BMFontConfiguration::parseConfigFile(const std::string& controlFile)
{
    Data data = tzw::Tfile::getInstance ()->getData (controlFile,false);

    if (memcmp("BMF", data.getBytes(), 3) == 0) {
        std::set<unsigned int>* ret = parseBinaryConfigFile(data.getBytes(), data.getSize(), controlFile);
        return ret;
    }

    auto contents = (const char*)data.getBytes();
    if (contents[0] == 0)
    {
        return nullptr;
    }

    std::set<unsigned int> *validCharsString = new std::set<unsigned int>();

    auto contentsLen = data.getSize();
    char line[512];

    auto next = strchr(contents, '\n');
    auto base = contents;
    int lineLength = 0;
    int parseCount = 0;
    while (next)
    {
        lineLength = ((int)(next - base));
        memcpy(line, contents + parseCount, lineLength);
        line[lineLength] = 0;

        parseCount += lineLength + 1;
        if (parseCount < contentsLen)
        {
            base = next + 1;
            next = strchr(base, '\n');
        }
        else
        {
            next = nullptr;
        }

        if (memcmp(line, "info face", 9) == 0)
        {
            // FIXME: info parsing is incomplete
            // Not needed for the Hiero editors, but needed for the AngelCode editor
            //            [self parseInfoArguments:line];
            this->parseInfoArguments(line);
        }
        // Check to see if the start of the line is something we are interested in
        else if (memcmp(line, "common lineHeight", 17) == 0)
        {
            this->parseCommonArguments(line);
        }
        else if (memcmp(line, "page id", 7) == 0)
        {
            this->parseImageFileName(line, controlFile);
        }
        else if (memcmp(line, "chars c", 7) == 0)
        {
            // Ignore this line
        }
        else if (memcmp(line, "char", 4) == 0)
        {
            // Parse the current line and create a new CharDef
            tFontDefHashElement* element = (tFontDefHashElement*)malloc( sizeof(*element) );
            this->parseCharacterDefinition(line, &element->fontDef);

            element->key = element->fontDef.charID;
            HASH_ADD_INT(_fontDefDictionary, key, element);

            validCharsString->insert(element->fontDef.charID);
        }
//        else if(line.substr(0,strlen("kernings count")) == "kernings count")
//        {
//            this->parseKerningCapacity(line);
//        }
        else if (memcmp(line, "kerning first", 13) == 0)
        {
            this->parseKerningEntry(line);
        }
    }

    return validCharsString;
}

std::set<unsigned int>* BMFontConfiguration::parseBinaryConfigFile(unsigned char* pData, unsigned long size, const std::string& controlFile)
{
    /* based on http://www.angelcode.com/products/bmfont/doc/file_format.html file format */

    set<unsigned int> *validCharsString = new set<unsigned int>();

    unsigned long remains = size;

    pData += 4; remains -= 4;

    while (remains > 0)
    {
        unsigned char blockId = pData[0]; pData += 1; remains -= 1;
        uint32_t blockSize = 0; memcpy(&blockSize, pData, 4);

        pData += 4; remains -= 4;

        if (blockId == 1)
        {
            /*
             fontSize       2   int      0
             bitField       1   bits     2  bit 0: smooth, bit 1: unicode, bit 2: italic, bit 3: bold, bit 4: fixedHeigth, bits 5-7: reserved
             charSet        1   uint     3
             stretchH       2   uint     4
             aa             1   uint     6
             paddingUp      1   uint     7
             paddingRight   1   uint     8
             paddingDown    1   uint     9
             paddingLeft    1   uint     10
             spacingHoriz   1   uint     11
             spacingVert    1   uint     12
             outline        1   uint     13 added with version 2
             fontName       n+1 string   14 null terminated string with length n
             */

            _padding.top = (unsigned char)pData[7];
            _padding.right = (unsigned char)pData[8];
            _padding.bottom = (unsigned char)pData[9];
            _padding.left = (unsigned char)pData[10];
        }
        else if (blockId == 2)
        {
            /*
             lineHeight 2   uint    0
             base       2   uint    2
             scaleW     2   uint    4
             scaleH     2   uint    6
             pages      2   uint    8
             bitField   1   bits    10  bits 0-6: reserved, bit 7: packed
             alphaChnl  1   uint    11
             redChnl    1   uint    12
             greenChnl  1   uint    13
             blueChnl   1   uint    14
             */

            uint16_t lineHeight = 0; memcpy(&lineHeight, pData, 2);
            _commonHeight = lineHeight;

            uint16_t scaleW = 0; memcpy(&scaleW, pData + 4, 2);
            uint16_t scaleH = 0; memcpy(&scaleH, pData + 6, 2);


            uint16_t pages = 0; memcpy(&pages, pData + 8, 2);
        }
        else if (blockId == 3)
        {
            /*
             pageNames 	p*(n+1) 	strings 	0 	p null terminated strings, each with length n
             */

            const char *value = (const char *)pData;

            _atlasName = controlFile;//FileUtils::getInstance()->fullPathFromRelativeFile(value, controlFile);
        }
        else if (blockId == 4)
        {
            /*
             id         4   uint    0+c*20  These fields are repeated until all characters have been described
             x          2   uint    4+c*20
             y          2   uint    6+c*20
             width      2   uint    8+c*20
             height     2   uint    10+c*20
             xoffset    2   int     12+c*20
             yoffset    2   int     14+c*20
             xadvance   2   int     16+c*20
             page       1   uint    18+c*20
             chnl       1   uint    19+c*20
             */

            unsigned long count = blockSize / 20;

            for (unsigned long i = 0; i < count; i++)
            {
                tFontDefHashElement* element = (tFontDefHashElement*)malloc( sizeof(*element) );

                uint32_t charId = 0; memcpy(&charId, pData + (i * 20), 4);
                element->fontDef.charID = charId;

                uint16_t charX = 0; memcpy(&charX, pData + (i * 20) + 4, 2);
                element->fontDef.rect.origin.setX (charX);

                uint16_t charY = 0; memcpy(&charY, pData + (i * 20) + 6, 2);
                element->fontDef.rect.origin.setY (charY);

                uint16_t charWidth = 0; memcpy(&charWidth, pData + (i * 20) + 8, 2);
                element->fontDef.rect.size.setX (charWidth);

                uint16_t charHeight = 0; memcpy(&charHeight, pData + (i * 20) + 10, 2);
                element->fontDef.rect.size.setY (charHeight);

                int16_t xoffset = 0; memcpy(&xoffset, pData + (i * 20) + 12, 2);
                element->fontDef.xOffset = xoffset;

                int16_t yoffset = 0; memcpy(&yoffset, pData + (i * 20) + 14, 2);
                element->fontDef.yOffset = yoffset;

                int16_t xadvance = 0; memcpy(&xadvance, pData + (i * 20) + 16, 2);
                element->fontDef.xAdvance = xadvance;

                element->key = element->fontDef.charID;
                HASH_ADD_INT(_fontDefDictionary, key, element);

                validCharsString->insert(element->fontDef.charID);
            }
        }
        else if (blockId == 5) {
            /*
             first  4   uint    0+c*10 	These fields are repeated until all kerning pairs have been described
             second 4   uint    4+c*10
             amount 2   int     8+c*10
             */

            unsigned long count = blockSize / 20;

            for (unsigned long i = 0; i < count; i++)
            {
                uint32_t first = 0; memcpy(&first, pData + (i * 10), 4);
                uint32_t second = 0; memcpy(&second, pData + (i * 10) + 4, 4);
                int16_t amount = 0; memcpy(&amount, pData + (i * 10) + 8, 2);

                tKerningHashElement *element = (tKerningHashElement *)calloc( sizeof( *element ), 1 );
                element->amount = amount;
                element->key = (first<<16) | (second&0xffff);
                HASH_ADD_INT(_kerningDictionary,key, element);
            }
        }

        pData += blockSize; remains -= blockSize;
    }

    return validCharsString;
}

void BMFontConfiguration::parseImageFileName(const char* line, const std::string& fntFile)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // page id=0 file="bitmapFontTest.png"
    //////////////////////////////////////////////////////////////////////////

    // page ID. Sanity check
    int pageId;
    sscanf(line, "page id=%d", &pageId);
    // file
    char fileName[255];
    sscanf(strchr(line,'"') + 1, "%[^\"]", fileName);
    _atlasName = tzw::Tfile::getInstance()->fullPathFromRelativeFile(fileName, fntFile);
}

void BMFontConfiguration::parseInfoArguments(const char* line)
{
    //////////////////////////////////////////////////////////////////////////
    // possible lines to parse:
    // info face="Script" size=32 bold=0 italic=0 charset="" unicode=1 stretchH=100 smooth=1 aa=1 padding=1,4,3,2 spacing=0,0 outline=0
    // info face="Cracked" size=36 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1
    //////////////////////////////////////////////////////////////////////////
    sscanf(strstr(line,"size=")+5,"%d",&m_fontSize);
    // padding
    sscanf(strstr(line,"padding=") + 8, "%d,%d,%d,%d", &_padding.top, &_padding.right, &_padding.bottom, &_padding.left);
    //CCLOG("cocos2d: padding: %d,%d,%d,%d", _padding.left, _padding.top, _padding.right, _padding.bottom);
}

void BMFontConfiguration::parseCommonArguments(const char* line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // common lineHeight=104 base=26 scaleW=1024 scaleH=512 pages=1 packed=0
    //////////////////////////////////////////////////////////////////////////

    // Height
    auto tmp = strstr(line, "lineHeight=") + 11;
    sscanf(tmp, "%d", &_commonHeight);
    // scaleW. sanity check
    int value;
    tmp = strstr(tmp, "scaleW=") + 7;
    sscanf(tmp, "%d", &value);

    // scaleH. sanity check
    tmp = strstr(tmp, "scaleH=") + 7;
    sscanf(tmp, "%d", &value);

    // pages. sanity check
    tmp = strstr(tmp, "pages=") + 6;
    sscanf(tmp, "%d", &value);

    // packed (ignore) What does this mean ??
}

void BMFontConfiguration::parseCharacterDefinition(const char* line, BMFontDef *characterDefinition)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // char id=32   x=0     y=0     width=0     height=0     xoffset=0     yoffset=44    xadvance=14     page=0  chnl=0
    //////////////////////////////////////////////////////////////////////////

    // Character ID
    auto tmp = strstr(line, "id=") + 3;
    sscanf(tmp, "%u", &characterDefinition->charID);

    // Character x
    tmp = strstr(tmp, "x=") + 2;
    float tmpFloat;
    sscanf(tmp, "%f", &tmpFloat);
    characterDefinition->rect.origin.setX (tmpFloat);
    // Character y
    tmp = strstr(tmp, "y=") + 2;
    sscanf(tmp, "%f", &tmpFloat);
    characterDefinition->rect.origin.setY (tmpFloat);
    // Character width
    tmp = strstr(tmp, "width=") + 6;
    sscanf(tmp, "%f", &tmpFloat);
    characterDefinition->rect.size.setX (tmpFloat);
    // Character height
    tmp = strstr(tmp, "height=") + 7;
    sscanf(tmp, "%f", &tmpFloat);
    characterDefinition->rect.size.setY (tmpFloat);
    // Character xoffset
    tmp = strstr(tmp, "xoffset=") + 8;
    sscanf(tmp, "%hd", &characterDefinition->xOffset);
    // Character yoffset
    tmp = strstr(tmp, "yoffset=") + 8;
    sscanf(tmp, "%hd", &characterDefinition->yOffset);
    // Character xadvance
    tmp = strstr(tmp, "xadvance=") + 9;
    sscanf(tmp, "%hd", &characterDefinition->xAdvance);
}

void BMFontConfiguration::parseKerningEntry(const char* line)
{
    //////////////////////////////////////////////////////////////////////////
    // line to parse:
    // kerning first=121  second=44  amount=-7
    //////////////////////////////////////////////////////////////////////////

    int first, second, amount;
    auto tmp = strstr(line, "first=") + 6;
    sscanf(tmp, "%d", &first);

    tmp = strstr(tmp, "second=") + 7;
    sscanf(tmp, "%d", &second);

    tmp = strstr(tmp, "amount=") + 7;
    sscanf(tmp, "%d", &amount);

    tKerningHashElement *element = (tKerningHashElement *)calloc( sizeof( *element ), 1 );
    element->amount = amount;
    element->key = (first<<16) | (second&0xffff);
    HASH_ADD_INT(_kerningDictionary,key, element);
}
