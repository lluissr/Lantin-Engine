#ifndef __Quadtree_h__
#define __Quadtree_h__

#include "MathGeoLib.h"
#include <list>

class GameObject;

class QuadtreeNode
{
public:
	QuadtreeNode();
	QuadtreeNode(const math::AABB& aabb);
	~QuadtreeNode();

	void InsertGameObject(GameObject* go);
	void RemoveGameObject(GameObject* go);
	void CreateChilds();
	void RedistributeGameObjects();
	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*>& objects, const TYPE& primitive) const;

	math::AABB aabb;
	std::list<GameObject*> gameObjects;
	QuadtreeNode* parent = nullptr;
	QuadtreeNode* childs[4];
};


class Quadtree
{
public:
	Quadtree();
	~Quadtree();

	void InitQuadTree(const math::AABB& aabb);
	void InsertGameObject(GameObject* go);
	void RemoveGameObject(GameObject* go);
	void Clear();
	template<typename TYPE>
	void CollectIntersections(std::vector<GameObject*>& objects, const TYPE& primitive) const;

	QuadtreeNode* root = nullptr;
};

template<typename TYPE>
inline void Quadtree::CollectIntersections(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	if (root != nullptr)
	{
		root->CollectIntersections(objects, primitive);
	}
}

template<typename TYPE>
inline void QuadtreeNode::CollectIntersections(std::vector<GameObject*>& objects, const TYPE & primitive) const
{
	if (primitive.Intersects(aabb))
	{
		for (std::list<GameObject*>::const_iterator it = this->gameObjects.begin(); it != this->gameObjects.end(); ++it)
		{
			if (primitive.Intersects((*it)->componentMesh->mesh->globalBoundingBox))
			{
				objects.push_back(*it);
			}

		}
		for (int i = 0; i < 4; ++i)
		{
			if (childs[i] != nullptr) childs[i]->CollectIntersections(objects, primitive);
		}
	}
}


#endif
