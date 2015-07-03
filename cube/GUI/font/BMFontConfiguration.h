#ifndef BMFONTCONFIGURATION_H
#define BMFONTCONFIGURATION_H
#include <string>
#include <set>
#include "geometry/Rect.h"
#include "external/UT_hash.h"
struct _FontDefHashElement;

/**
@struct BMFontDef
BMFont definition
*/
typedef struct _BMFontDef {
    //! ID of the character
    unsigned int charID;
    //! origin and size of the font
    Rect rect;
    //! The X amount the image should be offset when drawing the image (in pixels)
    short xOffset;
    //! The Y amount the image should be offset when drawing the image (in pixels)
    short yOffset;
    //! The amount to move the current position after drawing the character (in pixels)
    short xAdvance;
} BMFontDef;

/** @struct BMFontPadding
BMFont padding
@since v0.8.2
*/
typedef struct _BMFontPadding {
    /// padding left
    int    left;
    /// padding top
    int top;
    /// padding right
    int right;
    /// padding bottom
    int bottom;
} BMFontPadding;

typedef struct _FontDefHashElement
{
    unsigned int    key;        // key. Font Unicode value
    BMFontDef       fontDef;    // font definition
    UT_hash_handle  hh;
} tFontDefHashElement;

// Equal function for targetSet.
typedef struct _KerningHashElement
{
    int				key;        // key for the hash. 16-bit for 1st element, 16-bit for 2nd element
    int				amount;
    UT_hash_handle	hh;
} tKerningHashElement;


class  BMFontConfiguration
{
    // FIXME: Creating a public interface so that the bitmapFontArray[] is accessible
public://@public
    // BMFont definitions
    tFontDefHashElement *_fontDefDictionary;

    //! FNTConfig: Common Height Should be signed (issue #1343)
    int _commonHeight;
    //! Padding
    BMFontPadding    _padding;
    //! atlas name
    std::string _atlasName;
    //! values for kerning
    tKerningHashElement *_kerningDictionary;

    // Character Set defines the letters that actually exist in the font
    std::set<unsigned int> *_characterSet;
public:
    /**
     * @js ctor
     */
    BMFontConfiguration();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~BMFontConfiguration();
    /**
     * @js NA
     * @lua NA
     */
    std::string description() const;

    /** allocates a BMFontConfiguration with a FNT file */
    static BMFontConfiguration * create(const std::string& FNTfile);

    /** initializes a BitmapFontConfiguration with a FNT file */
    bool initWithFNTfile(const std::string& FNTfile);

    inline const std::string& getAtlasName(){ return _atlasName; }
    inline void setAtlasName(const std::string& atlasName) { _atlasName = atlasName; }

    std::set<unsigned int>* getCharacterSet() const;
    _BMFontDef getFontDef(int c);
    int fontSize() const;
    void setFontSize(int fontSize);

private:
    std::set<unsigned int>* parseConfigFile(const std::string& controlFile);
    std::set<unsigned int>* parseBinaryConfigFile(unsigned char* pData, unsigned long size, const std::string& controlFile);
    void parseCharacterDefinition(const char* line, BMFontDef *characterDefinition);
    void parseInfoArguments(const char* line);
    void parseCommonArguments(const char* line);
    void parseImageFileName(const char* line, const std::string& fntFile);
    void parseKerningEntry(const char* line);
    void purgeKerningDictionary();
    void purgeFontDefDictionary();
    int m_fontSize;
};

#endif
