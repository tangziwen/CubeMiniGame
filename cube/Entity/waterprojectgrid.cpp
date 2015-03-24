#include "waterprojectgrid.h"
#include "material/materialpool.h"
#include "texture/texturepool.h"
#include "geometry/ray.h"
WaterProjectGrid::WaterProjectGrid(float width, float height, float waterLevel, float normal_splat)
{
    initializeOpenGLFunctions ();

    initWaves();
    m_normalSplatSize = normal_splat;
    m_time = 0;
    m_width = width;
    m_height = height;
    m_waterLevel = waterLevel;
    m_mesh = new TMesh();
    //generate vertices

    //no need to set position in local ,be cause we will project it to world space later
    for(int i =0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {

            VertexData v;
            v.position = QVector3D(j- width/2,
                                   waterLevel,
                                   i - height/2);
            v.texCoord = QVector2D(j*1.0/width,i*1.0/height);
            m_mesh->pushVertex (v);
        }
    }


    //generate indices
    for(int i =0;i<height-1;i++)
    {
        for(int j = 0;j<width-1;j++)
        {
            m_mesh->pushIndex (i*width+j);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex (i*width+j+1);

            m_mesh->pushIndex (i*width+j+1);
            m_mesh->pushIndex ((i+1)*width+j);
            m_mesh->pushIndex ((i+1)*width+j+1);
        }
    }

    m_mesh->setMaterial (new Material());

    m_mesh->finishWithoutNormal ();

    addMesh (m_mesh);

    this->setShaderProgram (ShaderPool::getInstance()->get ("deferred_water_PG"));

    m_mesh->getMaterial ()->setNormalMap (TexturePool::getInstance ()->createOrGetTexture ("./res/texture/water/water_normal.png"));

    m_mirrorRenderTarget = NULL;
}

float WaterProjectGrid::width() const
{
    return m_width;
}

void WaterProjectGrid::setWidth(float width)
{
    m_width = width;
}
float WaterProjectGrid::height() const
{
    return m_height;
}

void WaterProjectGrid::setHeight(float height)
{
    m_height = height;
}

void WaterProjectGrid::adjustVertices()
{
    m_time +=0.05;
    getShaderProgram ()->setUniformFloat ("g_time",m_time);
    getShaderProgram ()->setUniformFloat ("g_normal_splat_size",m_normalSplatSize);
    getShaderProgram ()->setUniform3Float ("g_eye_dir",camera->getForwardVector ());
    QVector3D v0,v1,v2,v3;
    QMatrix4x4 vp;
    vp = camera->getProjection ()*camera->getViewMatrix ();

    float min_x = -1,min_y = -1,max_x = 1,max_y = 1;

    adjustMinMax(&min_x,&min_y,&max_x,&max_y);

    //检测NDC四个角发射的射线能否与水平面相交，当规范空间的四个角的射线能正确水平面相交时，才直接取点
    v0 = caculateWorldPosForLower(QVector2D(min_x,min_y),vp.inverted ());//bl
    v1 = caculateWorldPosForLower(QVector2D(max_x,min_y),vp.inverted ());//br
    v2 = caculateWorldPosUpper(QVector2D(min_x,max_y),vp.inverted ());//tl
    v3 = caculateWorldPosUpper(QVector2D(max_x,max_y),vp.inverted ());//tr

    AABB aabb;
    aabb.update (v0);
    aabb.update (v1);
    aabb.update (v2);
    aabb.update (v3);

    float du = 1.0f/float(m_width-1);
    float dv = 1.0f/float(m_height-1);
    float u,v=0.0f;

    for(int i =0;i<m_height;i++)
    {
        u = 0;
        for(int j = 0;j<m_width;j++)
        {
            float x = (1-u)*(1-v)*v2.x () + (1-u)*v*v0.x () + u*(1-v)*v3.x ()+ u*v*v1.x ();
            float z = (1-u)*(1-v)*v2.z () + (1-u)*v*v0.z () + u*(1-v)*v3.z ()+ u*v*v1.z();
            float y = 0;
            auto index = i*m_width+j;
            m_mesh->at (index)->position = QVector3D(x,y,z);

            u+= du;
        }
        v+=dv;
    }


    for(int i =0;i<m_height;i++)
    {
        for(int j=0;j<m_width;j++)
        {
            auto index = i*m_width+ j;
            auto y = 0.0f;
            for(int k =0;k<4;k++)
            {
                auto pos = m_mesh->at (index)->position;
                y += evaluateWave (m_waves[k],QVector2D(pos.x (),pos.z ()),m_time);
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


QVector3D  WaterProjectGrid::caculateWorldPosUpper(QVector2D vec, QMatrix4x4 VP_R)
{
    QVector3D nearP(vec.x(), vec.y(), -1.0f), farP(vec.x(), vec.y(), 1.0f);
    nearP = screenToWorld (nearP,VP_R);
    farP =screenToWorld (farP,VP_R);
    QVector3D dir(farP - nearP);
    dir.normalized ();
    Ray ray(nearP,dir);
    Plane plane(QVector3D(0,1,0),QVector3D(0,0,0));
    auto p = ray.intersectPlane (plane);
    p.setY (0);
    //第一种是，法线反向了，发生了BackFiring现象，这个时候，得到的点是处于该方向的反向延长线上，结果是不正确的。因此，
    //在这里我选择了该点投影到视锥体远端位置时，与视锥体远点底部顶点的连线。用该连线与水平面求交就可以获得合理，且铺满屏幕的

    //第二种可能是射线与发现有焦点，但是该焦点远远超出了视锥体空间外，比方说视线略方向低于但接近水平面方向时，
    //这时候，虽然可以构建出水面，但是因为水面的大小可能非常的大，以至于造成可见范围内的纹理的拉伸
    auto aabb = camera->FrustumAABB ();
    aabb.transForm (camera->getModelTrans ());
    auto newDir = p - nearP;
    if(QVector3D::dotProduct (newDir,dir)<=-0.1  || !aabb.isInside (p))
    {
        nearP = QVector3D (vec.x (),1,1.0f);
        farP = QVector3D(vec.x (),-1,1.0f);
        nearP = screenToWorld (nearP,VP_R);
        farP =screenToWorld (farP,VP_R);
        dir = (farP - nearP);
        dir.normalize ();
        auto dist=0.0f;
        auto ndd = QVector3D::dotProduct (QVector3D(0,1,0),dir);
        if(ndd == 0)
            dist=0.0f;
        auto ndo = QVector3D::dotProduct(QVector3D(0,1,0),nearP);
        dist= (0 - ndo) / ndd;
        p =   nearP + dist *  dir;
        return p;
    }
    else
    {
        return p;
    }
}

QVector3D WaterProjectGrid::caculateWorldPosForLower(QVector2D vec, QMatrix4x4 VP_R)
{
    QVector3D nearP(vec.x(), vec.y(), -1.0f), farP(vec.x(), vec.y(), 1.0f);
    nearP = screenToWorld (nearP,VP_R);
    farP =screenToWorld (farP,VP_R);
    QVector3D dir(farP - nearP);
    dir.normalized ();
    Ray ray(nearP,dir);
    Plane plane(QVector3D(0,1,0),QVector3D(0,0,0));
    auto p1 = ray.intersectPlane (plane);
    return p1;
}

float WaterProjectGrid::waterLevel() const
{
    return m_waterLevel;
}

void WaterProjectGrid::setWaterLevel(float waterLevel)
{
    m_waterLevel = waterLevel;
}

QVector3D WaterProjectGrid::screenToWorld(QVector3D v, QMatrix4x4 VP_R)
{
    QVector4D src4(v,1);
    QVector4D result4 = VP_R * src4;
    float w = result4.w ();
    QVector3D result3 = result4.toVector3D () / w;
    return result3;
}

float WaterProjectGrid::normalSplatSize() const
{
    return m_normalSplatSize;
}

void WaterProjectGrid::setNormalSplatSize(float normalSplatSize)
{
    m_normalSplatSize = normalSplatSize;
}

float WaterProjectGrid::getLowerBound()
{
    float result =0;
    for(int i =0;i<4;i++)
    {
        result += -1*m_waves[i].fAmp;
    }
    return result;
}

float WaterProjectGrid::getUpperBound()
{
    float result =0;
    for(int i =0;i<4;i++)
    {
        result += m_waves[i].fAmp;
    }
    return result;
}

float WaterProjectGrid::evaluateWave(Wave w, QVector2D vPos, float fTime)
{
    return w.fAmp * sin( QVector2D::dotProduct (w.vDir,vPos)* w.fFreq + fTime * w.fPhase );
}

void WaterProjectGrid::adjustMinMax(float *min_x, float *min_y, float *max_x, float *max_y)
{
    Ray ray_list[4];
    ray_list[0] = camera->screenToWorldRay (QVector2D(*min_x,*min_y));//bl
    ray_list[1] = camera->screenToWorldRay(QVector2D(*max_x,*min_y));//br
    ray_list[2] = camera->screenToWorldRay(QVector2D(*min_x,*max_y));//tl
    ray_list[3] = camera->screenToWorldRay(QVector2D(*max_x,*max_y));//tr

    for(int i =0;i<4;i++)
    {
        Plane upper(QVector3D(0,1,0),QVector3D(0,getUpperBound (),0));
        Plane lower(QVector3D(0,1,0),QVector3D(0,getLowerBound (),0));

        auto p1 = ray_list[i].intersectPlane (upper);
        auto p2 = ray_list[i].intersectPlane (lower);
        p1.setY (0);
        p2.setY (0);
        p1 = camera->worldToScreen (p1);
        p2 = camera->worldToScreen (p2);

        auto test_min_x = std::min(p1.x (),p2.x ());
        auto test_min_y = std::min(p1.y (),p2.y ());
        auto test_max_x = std::max(p1.x (),p2.x ());
        auto test_max_y = std::max(p1.y (),p2.y ());

        if((*min_x)>test_min_x) (*min_x) = test_min_x;
        if((*min_y)>test_min_y) (*min_y) = test_min_y;
        if((*max_x)<test_max_x) (*max_x) = test_max_x;
        if((*max_y)<test_max_y) (*max_y) = test_max_y;
    }
}

void WaterProjectGrid::initWaves()
{
    m_waves[0] = { 0.5f, 0.2f, 0.50f, QVector2D( -1.0f, 0.0f ) };
    m_waves[1] =  { 1.0f, 0.17f, 1.30f, QVector2D( -0.7f, 0.7f ) };
    m_waves[2] =  { 1.0f, 0.17f, 1.30f, QVector2D( 0.7f, 0.7f ) };
    m_waves[3] =  { .25f, 0.4f, 0.250f, QVector2D( 0.2f, 0.1f ) };
}
RenderTarget *WaterProjectGrid::mirrorRenderTarget() const
{
    return m_mirrorRenderTarget;
}

void WaterProjectGrid::setMirrorRenderTarget(RenderTarget *mirrorRenderTarget)
{
    m_mirrorRenderTarget = mirrorRenderTarget;
    m_mirrorRenderTarget->setIgnoreEntity (this);
    QMatrix4x4 matrix;
    matrix.scale (1,-1,1);
    m_mirrorRenderTarget->setAuxMatrix (matrix);
    m_mirrorRenderTarget->setIsEnableClipPlane (true);
    double clipPlane[4] = {0.0, -1.0, 0.0, 0.0};
    m_mirrorRenderTarget->setClipPlane (clipPlane);
}

TMesh *WaterProjectGrid::mesh() const
{
    return m_mesh;
}

void WaterProjectGrid::setMesh(TMesh *mesh)
{
    m_mesh = mesh;
}






