#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <string>
namespace tzw {
class  Data
{

public:
    /**
     * This parameter is defined for convenient reference if a null Data object is needed.
     */
    static const Data Null;

    /**
     * Constructor of Data.
     */
    Data();

    /**
     * Copy constructor of Data.
     */
    Data(const Data& other);

    /**
     * Copy constructor of Data.
     */
    Data(Data&& other);

    /**
     * Destructor of Data.
     */
    ~Data();

    /**
     * Overroads of operator=.
     */
    Data& operator= (const Data& other);

    /**
     * Overroads of operator=.
     */
    Data& operator= (Data&& other);

    /**
     * Gets internal bytes of Data. It will retrun the pointer directly used in Data, so don't delete it.
     *
     * @return Pointer of bytes used internal in Data.
     */
    unsigned char* getBytes() const;

    std::string getString();

    /**
     * Gets the size of the bytes.
     *
     * @return The size of bytes of Data.
     */
    size_t getSize() const;

    /** Copies the buffer pointer and its size.
     *  @note This method will copy the whole buffer.
     *        Developer should free the pointer after invoking this method.
     *  @see Data::fastSet
     */
    void copy(const unsigned char* bytes, const size_t size);

    /** Fast set the buffer pointer and its size. Please use it carefully.
     *  @param bytes The buffer pointer, note that it have to be allocated by 'malloc' or 'calloc',
     *         since in the destructor of Data, the buffer will be deleted by 'free'.
     *  @note 1. This method will move the ownship of 'bytes'pointer to Data,
     *        2. The pointer should not be used outside after it was passed to this method.
     *  @see Data::copy
     */
    void fastSet(unsigned char* bytes, const size_t size);

    /**
     * Clears data, free buffer and reset data size.
     */
    void clear();

    /**
     * Check whether the data is null.
     *
     * @return True if the the Data is null, false if not.
     */
    bool isNull() const;

	bool mangledForDDS;

private:
    void move(Data& other);

private:
    unsigned char* _bytes;
    size_t _size;
};

}

#endif // DATA_H
