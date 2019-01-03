#include "Quadtree.h"
#include "Globals.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"

#define MAX_ITEMS 8
#define MIN_SIZE 50.0f

Quadtree::Quadtree()
{
}


Quadtree::~Quadtree()
{
	Clear();
}

void Quadtree::InitQuadTree(const math::AABB& aabb)
{
	Clear();
	root = new QuadtreeNode(aabb);
}

void Quadtree::InsertGameObject(GameObject* go)
{
	if (root != nullptr && go->componentMesh->mesh->globalBoundingBox.Intersects(root->aabb))
	{
		root->InsertGameObject(go);
	}
}

void Quadtree::RemoveGameObject(GameObject * go)
{
	if (root != nullptr)
	{
		root->RemoveGameObject(go);
	}
}

void Quadtree::Clear()
{
	RELEASE(root);
}




/* Quadtre Node*/


QuadtreeNode::QuadtreeNode()
{
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}


QuadtreeNode::QuadtreeNode(const math::AABB& aabb) : aabb(aabb)
{
	childs[0] = nullptr;
	childs[1] = nullptr;
	childs[2] = nullptr;
	childs[3] = nullptr;
}


QuadtreeNode::~QuadtreeNode()
{
	RELEASE(childs[0]);
	RELEASE(childs[1]);
	RELEASE(childs[2]);
	RELEASE(childs[3]);
}

void QuadtreeNode::InsertGameObject(GameObject* go)
{
	gameObjects.push_back(go);

	if (!(childs[0] == nullptr && (gameObjects.size() < MAX_ITEMS || aabb.HalfSize().LengthSq() <= MIN_SIZE * MIN_SIZE)))
	{
		if (childs[0] == nullptr)
		{
			CreateChilds();
		}
		RedistributeGameObjects();
	}
}

void QuadtreeNode::RemoveGameObject(GameObject* go)
{
	std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), go);
	if (it != gameObjects.end())
	{
		gameObjects.erase(it);
	}

	if (childs[0] != nullptr)
	{
		for (int i = 0; i < 4; ++i)
		{
			childs[i]->RemoveGameObject(go);
		}
	}
}

void QuadtreeNode::CreateChilds()
{
	math::AABB newAABB;
	math::float3 aabbSize(aabb.Size());
	math::float3 newSize(aabbSize.x * 0.5f, aabbSize.y, aabbSize.z * 0.5f);
	math::float3 aabbCenter(aabb.CenterPoint());
	math::float3 newCenter(aabbCenter);

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[0] = new QuadtreeNode(newAABB);
	childs[0]->parent = this;

	newCenter.x = aabbCenter.x + aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[1] = new QuadtreeNode(newAABB);
	childs[1]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z - aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[2] = new QuadtreeNode(newAABB);
	childs[2]->parent = this;

	newCenter.x = aabbCenter.x - aabbSize.x * 0.25f;
	newCenter.z = aabbCenter.z + aabbSize.z * 0.25f;
	newAABB.SetFromCenterAndSize(newCenter, newSize);
	childs[3] = new QuadtreeNode(newAABB);
	childs[3]->parent = this;
}

void QuadtreeNode::RedistributeGameObjects()
{
	if (childs[0] != nullptr)
	{
		for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();)
		{
			bool intersects[4];
			for (int i = 0; i < 4; ++i)
			{
				intersects[i] = childs[i]->aabb.Intersects((*it)->componentMesh->mesh->globalBoundingBox);
			}

			if (intersects[0] && intersects[1] && intersects[2] && intersects[3])
			{
				++it;
			}
			else
			{
				for (int i = 0; i < 4; ++i)
				{
					if (intersects[i])
					{
						childs[i]->InsertGameObject(*it);
					}
				}
				it = gameObjects.erase(it);
			}
		}
	}
}