#ifndef TZW_SKY_H
#define TZW_SKY_H
#include "../Engine/EngineDef.h"
#include "../Interface/Drawable3D.h"
#include "../Mesh/Mesh.h"
#include "../Texture/TextureMgr.h"
#include "Model/Model.h"
namespace tzw{
	class Sky : public Drawable3D
	{
	public:
		Sky();
		bool isEnable() const;
		void setIsEnable(bool val);
		Material *getMaterial() const;
		virtual void setUpTransFormation(TransformationInfo & info);
		Mesh * getMesh();
		void prepare();
		float getWeather() const;
		void setWeather(float val);
	private:
		float m_weather;
		Model * m_model;
		Material * m_skyBoxTechnique;
		Texture * m_cloudTexture;
		bool m_isEnable;
		Texture * tintTex;
		Texture * tintTex2;
		Texture * sunTex;
		Texture  * moonTex;
		Texture * cloudTex;
		Texture * cloudTex2; 
		TZW_SINGLETON_DECL(Sky);
	};
}
#endif // !TZW_SKY_H
