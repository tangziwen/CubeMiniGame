#pragma once
namespace tzw
{
	class ShadowMapFBO
	{
	public:
		ShadowMapFBO();

		~ShadowMapFBO();

		bool Init(unsigned int WindowWidth, unsigned int WindowHeight);

		void BindForWriting();

		void BindForReading(unsigned TextureUnit);

	private:
		unsigned m_fbo;
		unsigned m_shadowMap;
		unsigned m_w;
		unsigned m_h;
	};
}
