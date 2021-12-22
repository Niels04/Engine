#pragma once

#include "Engine/ECS/Registry.hpp"
#include "Engine/objects/components.hpp"

namespace Engine
{
	class SceneGraphSystem : public System<5, 3>
	{
	public:
		SceneGraphSystem(Registry<5, 3>& reg);
		void setParent(Entity e, Entity newParent);
		void setParentRoot(Entity e);//makes the node a child of "root"
		void calculateGlobalTransforms();//the function traverses the whole tree and calculates all globalTransforms
		void onImGuiRender();
	private:
		virtual void onManagedEntityAdded(Entity e) override;
		virtual void onManagedEntityRemoved(Entity e) override;
		virtual void onSystemClear() override;
		struct Node
		{
			Node(Entity Entity, Node* Parent, size_t IndexInParent) : entity(Entity), parent(Parent), indexInParent(IndexInParent) {  }
			inline void orphan()
			{
				parent->children[indexInParent] = parent->children.back();//move back element in the parent's vector to the element that is to remove(this node)
				parent->children[indexInParent]->indexInParent = indexInParent;//assign the correct index to the moved node
				parent->children.pop_back();//pop the now obsolete back node(important to do this as the last step because we'd get an out of range error if the node to remove was the last one)
				parent = nullptr;
				indexInParent = invalidIndex;
			}
			inline void adopt(Node* child)
			{
				child->indexInParent = children.size();
				child->parent = this;
				children.push_back(child);
			}
			inline void destructSubtree()
			{
				for (auto& child : children)
				{
					child->destructSubtree();
					delete(child); child = nullptr;
				}
				children.clear();
			}
			Entity entity;
			Node* parent;
			size_t indexInParent;
			std::vector<Node*> children;
		};
		void calculateGlobalTransforms(Node* parent);
		void renderNode(const Node* n);
		void renderPropertyPanel(Entity e);
		Node m_tree;
		Registry<5, 3>& m_registry;
		std::vector<Node*> m_managedEntityToNode;
		Entity m_selected = 0;
	};
}