#include "waterNaive.h"
#include "material/materialpool.h"
#include "texture/texturepool.h"
Water::Water(float width, float height, float waterLevel, float scale)
{
    initWaves();
    m_time = 0;
    initializeOpenGLFunctions ();
    setWidth (width);
    setHeight (height);
    setWaterScale (scale);
    setWaterLevel (waterLevel);

    m_mesh = new TMesh();

    //generate vertices
    for(int i =0;i<width;i++)
    {
        for(int j=0;j<height;j++)
        {

            VertexData v;
            v.position = QVector3D(j*scale- height/2*scale,
                                   waterLevel,
                                   i*scale - width/2*scale);
            v.texCoord = QVector2D(j*1.0/width,i*1.0/height);
            m_mesh->pushVertex (v);
        }
    }



    //generate indices
    for(int i =0;i<width-1;i++)
    {
        for(int j = 0;j<height-1;j++)
        {
            m_mesh->pushIndex (i*width+j);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex (i*width+j+1);

            m_mesh->pushIndex (i*width+j+1);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex ((i+1)*width+j+1);
        }
    }

    m_mesh->setMaterial (MaterialPool::getInstance ()->createOrGetMaterial ("default"));

    m_mesh->finishWithoutNormal ();

    addMesh (m_mesh);

    this->setShaderProgram (ShaderPool::getInstance()->get ("deferred_water"));

    m_mesh->getMaterial ()->setNormalMap (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/water/water_normal.jpg"));

    m_mirrorRenderTarget = NULL;
    m_mirrorCamera = new Camera();
    m_mirrorCamera->setOrtho (-width*1.0/2,width*1.0/2,-height*1.0/2,height*1.0/2,0.01,100);
    m_mirrorCamera->setRotation (QVector3D(90,0,0));
    m_mirrorCamera->setName ("heihei");
}

float Water::width() const
{
    return m_width;
}

void Water::setWidth(float width)
{
    m_width = width;
}

float Water::height() const
{
    return m_height;
}

void Water::setHeight(float height)
{
    m_height = height;
}
float Water::waterScale() const
{
    return m_waterScale;
}

void Water::setWaterScale(float scale)
{
    m_waterScale = scale;
}
TMesh *Water::mesh() const
{
    return m_mesh;
}

void Water::setMesh(TMesh *mesh)
{
    m_mesh = mesh;
}
float Water::waterLevel() const
{
    return m_waterLevel;
}

void Water::setWaterLevel(float waterLevel)
{
    m_waterLevel = waterLevel;
}

float Water::evaluateWave(Wave w, QVector2D vPos, float fTime)
{
    return w.fAmp * sin( QVector2D::dotProduct (w.vDir,vPos)* w.fFreq + fTime * w.fPhase );
}

void Water::adjustVertices()
{
    m_time +=0.05;
    getShaderProgram ()->setUniformFloat ("g_time",m_time);
    for(int i =0;i<m_width;i++)
    {
        for(int j=0;j<m_height;j++)
        {
            auto index = i+ j*m_width;
            auto y = m_waterLevel;
            for(int k =0;k<4;k++)
            {
                y += evaluateWave (m_waves[k],QVector2D(i,j),m_time);
            }
            m_mesh->at (index)->position.setY (y);
        }
    }
    if(m_mirrorRenderTarget)
    {
        getShaderProgram ()->setUniformInteger ("g_has_mirror",1);
        getShaderProgram ()->setUniformInteger ("g_mirror_map",4);
        glActiveTexture (GL_TEXTURE4);
        glBindTexture (GL_TEXTURE_2D,m_mirrorRenderTarget->resultBuffer ()->texture ());
    }
    else
    {
        getShaderProgram ()->setUniformInteger ("g_has_mirror",0);
    }
    m_mesh->finishWithoutNormal ();
}

void Water::initWaves()
{
    m_waves[0] = { 0.5f, 0.2f, 0.50f, QVector2D( -1.0f, 0.0f ) };
   m_waves[1] =  { 1.0f, 0.17f, 1.30f, QVector2D( -0.7f, 0.7f ) };
   m_waves[2] =  { 1.0f, 0.17f, 1.30f, QVector2D( 0.7f, 0.7f ) };
   m_waves[3] =  { .25f, 0.4f, 0.250f, QVector2D( 0.2f, 0.1f ) };
}
Camera *Water::mirrorCamera() const
{
    return m_mirrorCamera;
}

void Water::setMirrorCamera(Camera *mirrorCamera)
{
    m_mirrorCamera = mirrorCamera;
}


RenderTarget *Water::mirrorRenderTarget() const
{
    return m_mirrorRenderTarget;
}

void Water::setMirrorRenderTarget(RenderTarget *mirrorRenderTarget)
{
    m_mirrorRenderTarget = mirrorRenderTarget;
    m_mirrorRenderTarget->setIgnoreEntity (this);
}







