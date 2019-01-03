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

	QuadtreeNode* root = nullptr;
};


#endif
