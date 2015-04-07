#include "OCTree.h"

static QVector3D offsetList[]={
    QVector3D(0,0,0),QVector3D(0,0.5,0),QVector3D(0,0,0.5),QVector3D(0.5,0.5,0),
    QVector3D(0.5,0.5,0.5),QVector3D(0,0.5,0.5),QVector3D(0.5,0,0.5),QVector3D(0.5,0,0)
};
void OCTree::init(float x, float y,  float z)
{
    init(QVector3D(x,y,z));
}

void OCTree::init(QVector3D size)
{
    m_root = new OCTreeNode(QVector3D(-1.0*size.x ()/2,-1.0*size.y ()/2,-1.0*size.z ()),size,NULL);

}

void OCTree::visitByFrustm(Frustum &frustum, std::vector<Entity *> *entity_list)
{
    m_root->visit (frustum,entity_list);
}



OCTreeNode::OCTreeNode(QVector3D orgin, QVector3D size, OCTreeNode *parent)
{
    m_parent = parent;
    QVector3D corner[8];
    corner[0] = orgin;
    corner[1] = orgin + QVector3D(0,size.y (),0);
    corner[2] = orgin + QVector3D(0,0,size.z ());
    corner[3] = orgin + QVector3D(size.x (),size.y (),0);
    corner[4] = orgin + QVector3D(size.x (),size.y (),size.z ());
    corner[5] = orgin + QVector3D(0,size.y (),size.z ());
    corner[6] = orgin + QVector3D(size.x (),0,size.z ());
    corner[7] = orgin + QVector3D(size.x (),0,0);

    m_aabb.update (corner,8);

    m_origin = orgin;
    m_size = size;

}


bool OCTreeNode::addEntity(Entity *entity)
{
    if(!m_aabb.isCanCotain (entity->getAABB ())) return false;
    if(getHierarchyLevel ()>=OCTREE_MAX_LEVEL)
    {
        m_entityList.push_back (entity);
        return true;
    }
    for(int i =0;i<8;i++)
    {
        if(m_children[i])
        {
            bool result = false;
            result = m_children[i]->addEntity (entity);
            if(result)
            {
                return true;
            }
        }else // maybe we need create new one.
        {
            //根据起始点的偏移位置创建一个新节点
            auto offset = offsetList[i];
            auto hs = m_size/2;
            auto test_node = new OCTreeNode(m_origin+QVector3D(offset.x ()*hs.x (),offset.y ()*hs.y(),offset.z()*hs.z ()),hs,this);
            //只有当新节点能够存放entity的时候，才将新节点作为子节点，否则删除。
            if(test_node->addEntity (entity))
            {
                m_children[i] = test_node;
                return true;
            }
            else
            {
                delete test_node;
            }
        }
    }
    //子节点都无法装下，那么直接放置在该处
    m_entityList.push_back (entity);
    return true;
}

int OCTreeNode::getHierarchyLevel()
{
    int result = 1;
    auto tmp =m_parent;
    while(tmp)
    {
        tmp = tmp->parent ();
        result++;
    }
    return result;
}
OCTreeNode *OCTreeNode::parent() const
{
    return m_parent;
}

void OCTreeNode::setParent(OCTreeNode *parent)
{
    m_parent = parent;
}

void OCTreeNode::visit(Frustum &frustum,std::vector<Entity *> * entity_list)
{
    if(frustum.isOutOfFrustum (m_aabb)) return;

    for(int i =0;i<8;i++)
    {
        if(m_children[i])
        {
            m_children[i]->visit (frustum,entity_list);
        }
    }
    for(int i =0;i<m_entityList.size ();i++)
    {
        entity_list->push_back (m_entityList[i]);
    }
}

