#ifndef TZW_RENDERBACKEND_H
#define TZW_RENDERBACKEND_H
#include "../Rendering/RenderFlag.h"
#include "RenderBackEndBase.h"
#include <string>
#include "DeviceTexture.h"
namespace tzw {

class RenderBackEnd : public RenderBackEndBase
{
public:
    static RenderBackEnd *shared();
    void initDevice(GLFWwindow *window) override;
    unsigned int genBuffer();
	void deleteBuffer(unsigned int index);
    void bindBuffer(RenderFlag::BufferTarget target,unsigned int handle);
    void submit(RenderFlag::BufferTarget target, unsigned int size, const void * data, RenderFlag::BufferStorageType storageType = RenderFlag::BufferStorageType::STATIC_DRAW);
	void resubmit(RenderFlag::BufferTarget target,unsigned int offset, unsigned int size, const void * data);
    void activeTextureUnit(unsigned int id);
    void enableFunction(RenderFlag::RenderFunction state);
    void disableFunction(RenderFlag::RenderFunction state);
    void setTexMIN(unsigned int textureID, int param, RenderFlag::TextureType type);
    void setTexMAG(unsigned int textureID, int param, RenderFlag::TextureType type);
    void bindTexture2DAndUnit(unsigned int texUnitID,unsigned int textureID,RenderFlag::TextureType type = RenderFlag::TextureType::Texture2D);
    void drawElement(RenderFlag::IndicesType type,unsigned int size, const void *indicesAddress);
	void drawElementInstanced(RenderFlag::IndicesType type, unsigned int size, const void *indicesAddress, int count);
    void setDepthTestMethod(const RenderFlag::DepthTestMethod & method);
    void setTextureWarp(unsigned int textureID, RenderFlag::WarpAddress warpAddress, RenderFlag::TextureType type);
    void bindFrameBuffer(unsigned int frameBufferID);
	void setBlendEnable(bool isEnable);
	void clear();
    void blitFramebuffer(int srcX0,
                           int srcY0,
                           int srcX1,
                           int srcY1,
                           int dstX0,
                           int dstY0,
                           int dstX1,
                           int dstY1);

    void setDepthTestEnable(bool isEnable);
    void setBlendEquation(RenderFlag::BlendingEquation equation);
    void setBlendFactor(RenderFlag::BlendingFactor factorSrc, RenderFlag::BlendingFactor factorDst);
    void setDepthMaskWriteEnable(bool isEnable);
    void genMipMap(unsigned int texUnitID);
	void setClearColor(float r, float g, float b);
	void setIsCullFace(bool val);
	bool getIsCullFace();
	void deleteFramebuffers(unsigned count, unsigned * obj);
	void DeleteTextures(unsigned count, unsigned * obj);
	void GenFramebuffers(unsigned count, unsigned * obj);
	void GenTextures(unsigned count, unsigned * obj);
	void TexImage2D(unsigned target, int level, int internalformat, int width, int height, int border, unsigned format, unsigned type, const void *pixels);
	void checkGL(int except_val = -1);
	void selfCheck();
	void setIsCheckGL(bool newVal);
	bool isCheckGL();
	std::string getCurrVersion();
	std::string getShaderSupportVersion();
	void printFullDeviceInfo();
    DeviceTexture * loadTexture_imp(const unsigned char* buf, size_t buffSize, unsigned int loadingFlag) override;
    DeviceTexture * loadTextureRaw_imp(const unsigned char* buf, int width, int height, ImageFormat format, unsigned int loadingFlag) override;
    DeviceShader * createShader_imp() override;
    DeviceBuffer * createBuffer_imp() override;
	void prepareFrame() override;
	void endFrame() override;
private:
	bool m_isCheckGL;
    static RenderBackEnd * m_instance;
    RenderBackEnd();
    //some state cache
    RenderFlag::DepthTestMethod m_depthTestMethodCache;
	bool m_isCullFace;
    unsigned int m_textureSlot[8];
	RenderFlag::BlendingFactor m_curBlendSrc;
	RenderFlag::BlendingFactor m_curBlendDst;
};

} // namespace tzw

#endif // TZW_RENDERBACKEND_H
