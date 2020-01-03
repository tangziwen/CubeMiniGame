#pragma once
#include "../Interface/Drawable3D.h"
#include "../Texture/TextureMgr.h"
namespace tzw
{
	class FrameBuffer;
	class ThumbNail
	{
	public:
		ThumbNail(Drawable3D * node);
		void doSnapShot();
		Drawable3D* getNode() const;
		void setNode(Drawable3D* const node);
		bool isIsDone() const;
		void setIsDone(const bool isDone);
		Texture* getTexture() const;
		void setTexture(Texture* const texture);
	protected:
		void initTexture();
		FrameBuffer * m_frameBuffer;
		Drawable3D * m_node;
		bool m_isDone;
	    Texture * m_texture;
	};

} // namespace tzw
