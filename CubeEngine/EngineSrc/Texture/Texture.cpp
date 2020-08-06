#include "Texture.h"
#include "BackEnd/RenderBackEnd.h"
#include "Engine/Engine.h"
#include "GL/glew.h"
#include "Utility/log/Log.h"
#include "Engine/WorkerThreadSystem.h"
#include "External/SOIL2/SOIL2.h"
#include "Utility/file/Tfile.h"
#include <algorithm>
namespace tzw
{
	Texture::Texture(): m_isLoaded(false)
	{
	}

	Texture::Texture(std::string filePath)
	{
		auto data =Tfile::shared()->getData(filePath,false);
		unsigned short loadingFlag = SOIL_FLAG_INVERT_Y;
		auto exten = Tfile::shared()->getExtension(filePath);
		for(auto& c : exten)
		{
		   c = tolower(c);
		}
		m_isHaveMipMap = false;
		bool isDDS = !exten.compare("dds") ||data.mangledForDDS;
		if(isDDS)
		{
			loadingFlag = SOIL_FLAG_DDS_LOAD_DIRECT;
		}
		else
		{
			loadingFlag = SOIL_FLAG_INVERT_Y;
		}
		this->m_textureId =  Engine::shared()->getRenderBackEnd()->loadTexture_imp(data.getBytes(),data.getSize(),loadingFlag);//SOIL_load_OGL_texture_from_memory(data.getBytes(),data.getSize(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, loadingFlag, &info);
		m_type = RenderFlag::TextureType::Texture2D;
		m_width = this->m_textureId->m_metaInfo.width;
		m_height = this->m_textureId->m_metaInfo.height;
		if(isDDS )
		{
			if(m_textureId->m_metaInfo.dds_mipMapLevel > 0)
			{
				m_isHaveMipMap = true;
			}else
			{
				m_isHaveMipMap = false;
			}
		}
		if (!m_textureId)
		{
			
			tlog("texture create failed, no such file %s %s\n", filePath.c_str(),SOIL_last_result());
			exit(0);
		}
		initData();
		m_isLoaded = true;
	}

	Texture::Texture(std::string filePath, char faceMode[6])
	{
		/*
		std::string resultFilePath = Engine::shared()->getFilePath(filePath);
		this->m_textureId = SOIL_load_OGL_single_cubemap(
			resultFilePath.c_str(), faceMode, SOIL_LOAD_AUTO, 0, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		m_type = RenderFlag::TextureType::TextureCubeMap;
		if (!m_textureId)
		{
			tlog("texture create failed, no such file %s %s\n", filePath.c_str(), SOIL_last_result());
			abort();
		}
		initData();
		m_isLoaded = true;
		*/
	}

	Texture::Texture(unsigned char* rawData, int w, int h,ImageFormat format, bool needFlipY)
	{
		this->m_textureId =  Engine::shared()->getRenderBackEnd()->loadTextureRaw_imp(rawData,w, h, format, 0);
		m_type = RenderFlag::TextureType::Texture2D;
		m_width = w;
		m_height = h;
		initData();
		m_isLoaded = true;
	}

	Texture::Texture(std::string PosXFilename,
					std::string NegXFilename,
					std::string PosYFilename,
					std::string NegYFilename,
					std::string PosZFilename,
					std::string NegZFilename)
	{
		/*
		this->m_textureId = SOIL_load_OGL_cubemap(PosXFilename.c_str(),
												NegXFilename.c_str(),
												PosYFilename.c_str(),
												NegYFilename.c_str(),
												PosZFilename.c_str(),
												NegZFilename.c_str(),
												3,
												0,
												SOIL_FLAG_TEXTURE_REPEATS);
		m_type = RenderFlag::TextureType::TextureCubeMap;
		initData();
		m_isLoaded = true;
		*/
	}

	void
	Texture::setFilter(Texture::FilterType type, int filter)
	{
		if(Engine::shared()->getRenderDeviceType() != RenderDeviceType::OpenGl_Device)
			return;
		switch (filter)
		{
		case 0:
			setMinFilter(type);
			setMagFilter(type);
			break;
		case 1:
			setMinFilter(type);
			break;
		case 2:
			setMagFilter(type);
			break;
		}
	}

	void Texture::setWarp(RenderFlag::WarpAddress warpAddress)
	{
		if(Engine::shared()->getRenderDeviceType() == RenderDeviceType::OpenGl_Device)
		{
			RenderBackEnd::shared()->setTextureWarp(m_textureId->m_uid, warpAddress, m_type);
		}
		
	}

	void Texture::loadAsync(std::string filePath, std::function<void(Texture*)> onFinish)
	{
		/*
		std::string resultFilePath = Engine::shared()->getFilePath(filePath);

		auto finishCB = [this,onFinish]
		{
			m_textureId = SOIL_create_OGL_texture(m_imgData, &m_width, &m_height, m_channel, 0, SOIL_FLAG_INVERT_Y);
			m_isLoaded = true;
			onFinish(this);
		};
		auto doCB = [=]
		{
			m_channel = 0;
			m_imgData = SOIL_load_image(resultFilePath.c_str(), &m_width, &m_height, &m_channel, SOIL_LOAD_AUTO);
		};
		m_type = RenderFlag::TextureType::Texture2D;
		WorkerThreadSystem::shared()->pushOrder(WorkerJob(doCB, finishCB));
		*/
	}

	DeviceTexture *
	Texture::handle()
	{
		return m_textureId;
	}

	vec2
	Texture::getSize()
	{
		return vec2(m_width, m_height);
	}

	RenderFlag::TextureType
	Texture::getType() const
	{
		return m_type;
	}

	void
	Texture::setMinFilter(Texture::FilterType t)
	{
		switch (t)
		{
		case FilterType::Linear:
			RenderBackEnd::shared()->setTexMIN(m_textureId->m_uid, GL_LINEAR, m_type);
			break;
		case FilterType::Nearest:
			RenderBackEnd::shared()->setTexMIN(m_textureId->m_uid, GL_NEAREST, m_type);
			break;
		case FilterType::LinearMipMapNearest:
			RenderBackEnd::shared()->setTexMIN(
				m_textureId->m_uid, GL_LINEAR_MIPMAP_NEAREST, m_type);
			break;
		case FilterType::LinearMipMapLinear:
			RenderBackEnd::shared()->setTexMIN(
				m_textureId->m_uid, GL_LINEAR_MIPMAP_LINEAR, m_type);
			break;
		case FilterType::NearestMipMapLinear:
			RenderBackEnd::shared()->setTexMIN(
				m_textureId->m_uid, GL_NEAREST_MIPMAP_LINEAR, m_type);
			break;
		}
	}

	void
	Texture::setMagFilter(Texture::FilterType t)
	{
		switch (t)
		{
		case FilterType::Linear:
			RenderBackEnd::shared()->setTexMAG(m_textureId->m_uid, GL_LINEAR, m_type);
			break;
		case FilterType::Nearest:
			RenderBackEnd::shared()->setTexMAG(m_textureId->m_uid, GL_NEAREST, m_type);
			break;
		case FilterType::LinearMipMapNearest:
			RenderBackEnd::shared()->setTexMAG(
				m_textureId->m_uid, GL_LINEAR_MIPMAP_NEAREST, m_type);
			break;
		case FilterType::LinearMipMapLinear:
			RenderBackEnd::shared()->setTexMAG(
				m_textureId->m_uid, GL_LINEAR_MIPMAP_LINEAR, m_type);
			break;
		case FilterType::NearestMipMapLinear:
			RenderBackEnd::shared()->setTexMAG(
				m_textureId->m_uid, GL_NEAREST_MIPMAP_LINEAR, m_type);
			break;
		}
	}

	DeviceTexture * Texture::getTextureId() const
	{
		return m_textureId;
	}

	void Texture::setTextureId(DeviceTexture * textureId)
	{
		m_textureId = textureId;
	}

	bool
	Texture::getIsHaveMipMap() const
	{
		return m_isHaveMipMap;
	}

	void
	Texture::genMipMap()
	{
		if(Engine::shared()->getRenderDeviceType() == RenderDeviceType::Vulkan_Device) return;
		glBindTexture(GL_TEXTURE_2D, m_textureId->m_uid);
		if (!m_isHaveMipMap)
		{
			RenderBackEnd::shared()->genMipMap(m_textureId->m_uid);
		}
		setFilter(FilterType::Linear, 2);
		setFilter(FilterType::LinearMipMapLinear, 1);
		m_isHaveMipMap = true;
	}

	void
	Texture::initData()
	{
		setWarp(RenderFlag::WarpAddress::Repeat);
		//setWarp(RenderFlag::WarpAddress::Clamp);
		
	}
} // namespace tzw
