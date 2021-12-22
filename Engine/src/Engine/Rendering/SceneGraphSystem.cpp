#include "Engpch.hpp"

#include "SceneGraphSystem.hpp"

#include "imgui.h"

namespace Engine
{
	SceneGraphSystem::SceneGraphSystem(Registry<5, 3>& reg)
		: m_registry(reg), m_tree(invalidIndex, nullptr, invalidIndex)//the root node doesn't have an entity attached to it, neighter does it have a parent
	{
		setRequirements<TransformComponent>();//the sceneGraphSystem works on all entities that have a transformComponent
	}
	void SceneGraphSystem::setParent(Entity e, Entity newParent)
	{
		Node* node = m_managedEntityToNode[getManagedEntityFromEntity(e)];//SHOULDN'T WE FIRST INDEX INTO m_entityToManagedEntity???
		node->orphan();//orphan the node from its old parent
		Node* parent = m_managedEntityToNode[getManagedEntityFromEntity(newParent)];
		parent->adopt(node);//adopt the node to its new parent
	}
	void SceneGraphSystem::setParentRoot(Entity e)
	{
		Node* node = m_managedEntityToNode[getManagedEntityFromEntity(e)];
		node->orphan();
		m_tree.adopt(node);
	}
	void SceneGraphSystem::calculateGlobalTransforms()//the function traverses the whole tree and calculates all globalTransforms
	{
		for (auto& child : m_tree.children)
		{
			auto& Childtransform = m_registry.get<TransformComponent>(child->entity);
			Childtransform.global = Childtransform.local;//because this is the first layer the globalTransform is just the local one
			calculateGlobalTransforms(child);
		}
	}
	void SceneGraphSystem::calculateGlobalTransforms(Node* parent)
	{
		for (auto& child : parent->children)
		{
			auto& Childtransform = m_registry.get<TransformComponent>(child->entity);
			Childtransform.global = m_registry.get<TransformComponent>(parent->entity).global * Childtransform.local;
			calculateGlobalTransforms(child);//recursively calls itself until it reaches the leaf nodes
		}
	}
	void SceneGraphSystem::onManagedEntityAdded(Entity e)
	{
		m_tree.children.push_back(new Node(e, &m_tree, m_tree.children.size()));
		m_managedEntityToNode.push_back(m_tree.children.back());
	}
	void SceneGraphSystem::onManagedEntityRemoved(Entity e)
	{
		//NOTE THAT IN ORDER TO DO THIS PROPERLY WE'D have to apply the deleted node's transform to all of it's children's local transform first(don't do this by simply applying the matrix, actually transfer the rotation and translation values)
		Index index = getManagedEntityFromEntity(e);
		Node* node = m_managedEntityToNode[index];
		auto& parentTransform = m_registry.get<TransformComponent>(e);
		for (auto& child : node->children)//assign all of the deleted node's children its parent node
		{
			//first adjust the child's transform, so that it stays at the same position in the world WE IGNORE SHEAR HERE
			/*auto& childTransform = m_registry.get<TransformComponent>(child->entity);
			childTransform.setPos(childTransform.getPos() + parentTransform.getPos());
			childTransform.setRot(childTransform.getRot() + parentTransform.getRot());
			childTransform.setScale(childTransform.getScale() * parentTransform.getScale());*/
			child->orphan();
			node->parent->adopt(child);
		}
		m_managedEntityToNode[index] = m_managedEntityToNode.back();
		m_managedEntityToNode.pop_back();
	}
	void SceneGraphSystem::onSystemClear()
	{
		//destroy the whole tree
		for (auto& child : m_tree.children)
		{
			child->destructSubtree();
			delete(child); child = nullptr;
		}
		m_tree.children.clear();
		m_managedEntityToNode.clear();
		m_selected = 0;
	}
	
	void SceneGraphSystem::onImGuiRender()
	{
		ImGui::Begin("Scene Graph");
		for (const auto& child : m_tree.children)
		{
			renderNode(child);
		}
		ImGui::End();
		renderPropertyPanel(m_selected);
	}

	void SceneGraphSystem::renderNode(const Node* n)
	{
		char name[10] = { ' ' };
		sprintf(name, "%u", (uint32_t)n->entity);
		ImGuiTreeNodeFlags flags = ((m_selected == n->entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;//if the node is the currently selected node draw it as a selected node
		bool isOpen = ImGui::TreeNodeEx(name, flags);
		if (ImGui::IsItemClicked())//if the user clicks on the node set it to be the selected node
			m_selected = n->entity;
		if (isOpen)
		{
			//draw contents:
			ImGui::Text("Entity: %u", n->entity);
			//display a window that lets the user change the node's parent
			static int32_t parentEntity = -1;
			if (ImGui::Button("set parent", { 80, 24 }))
			{
				ImGui::OpenPopup("popup_setParent");
				parentEntity = static_cast<int32_t>(n->parent->entity);//when opening the window the value displayed in the input field is set to the current parent
				//if "n->parent->entity" is "invalidIndex", this becomes -1 -> -1 means that the parentNode doesn't have an entity attached to it(is the root node)
			}
			if (ImGui::BeginPopupModal("popup_setParent"))
			{
				ImGui::Text("Set parent");
				ImGui::InputInt("parent:", &parentEntity, 1, 1);
				if (ImGui::Button("Apply"))
				{
					if (parentEntity != n->entity)//don't allow for a node to be it's own parent
					{//THEORETICALLY WE'D HAVE TO MAKE A CHECK HERE IF THE NEW PARENT IS SOMEWHERE IN THE SUBTREE OF THE NODE ITSELF BECAUSE THAT SHOULDN'T BE ALLOWED -> starting from the new parent go up the tree
						//layers until the root is reached. If the node that we want to set as a child of the new parent node is somewhere in that path, then the operation shouldn't be executed
						if (parentEntity == -1)
							setParentRoot(n->entity);
						else
							setParent(n->entity, static_cast<Entity>(parentEntity));
					}
				}
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			//draw children:
			for (const auto& child : n->children)
			{
				renderNode(child);//calls itself recursively until leaf nodes are reached
			}
			ImGui::TreePop();
		}
	}

	void SceneGraphSystem::renderPropertyPanel(Entity e)
	{
		ImGui::Begin("Properties");
		if (m_registry.hasComponent<TransformComponent>(e))//technically we didn't have to do this check, so maybe just remove it
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.3086f, 0.0977f, 0.8867f, 1.0f });
			ImGui::Text("Transform:");
			ImGui::PopStyleColor();
			auto& transform = m_registry.get<TransformComponent>(e);
			vec3 scale = transform.getScale();
			vec3 pos = transform.getPos();
			vec3 rot = transform.getRot();
			ImGui::SliderFloat3("scale", (float*)&scale, 0.1f, 10.0f);
			ImGui::DragFloat3("position", (float*)&pos, 0.25f, -100.0f, 100.0f);
			ImGui::DragFloat3("rotation", (float*)&rot, 0.25f, -2.0f * 3.1415926f, 2.0f * 3.1415926f);
			transform.setScale(scale);
			transform.setPos(pos);
			transform.setRot(rot);
		}
		if (m_registry.hasComponent<MeshComponent>(e))
		{
			auto& Mesh = m_registry.get<MeshComponent>(e);
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.1641f, 0.8879f, 0.0977f, 1.0f });
			ImGui::Text("Mesh: %s", Mesh.getName().c_str());
			ImGui::PopStyleColor();
			auto& Material = Mesh.getMaterial();
			ImGui::PushStyleColor(ImGuiCol_Text, { 0.8867f, 0.6285f, 0.0977f, 1.0f });
			ImGui::Text("Material: %s", Material->getName().c_str());
			ImGui::PopStyleColor();
			material::renderMaterial(Material);
		}
		if (m_registry.hasComponent<DirLightComponent>(e))
		{
			auto& light = m_registry.get<DirLightComponent>(e);
			ImGui::Text("Directional light:");
			ImGui::ColorEdit3("ambient", (float*)&light.ambient);
			ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
			ImGui::ColorEdit3("specular", (float*)&light.specular);
			ImGui::Text("direction: %.2f|%.2f|%.2f", light.direction.x, light.direction.y, light.direction.z);
		}
		if (m_registry.hasComponent<PointLightComponent>(e))
		{
			auto& light = m_registry.get<PointLightComponent>(e);
			ImGui::Text("Point light:");
			ImGui::ColorEdit3("ambient", (float*)&light.ambient);
			ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
			ImGui::ColorEdit3("specular", (float*)&light.specular);
			ImGui::Text("position: %.2f|%.2f|%.2f", light.position.x, light.position.y, light.position.z);
		}
		if (m_registry.hasComponent<SpotLightComponent>(e))
		{
			auto& light = m_registry.get<SpotLightComponent>(e);
			ImGui::Text("Spot light:");
			ImGui::ColorEdit3("ambient", (float*)&light.ambient);
			ImGui::ColorEdit3("diffuse", (float*)&light.diffuse);
			ImGui::ColorEdit3("specular", (float*)&light.specular);
			ImGui::SliderFloat("cutoff", &light.cutOff, cosf(80.0f * (3.1415926f / 180.0f)), 1.0f);
			ImGui::Text("direction: %.2f|%.2f|%.2f", light.direction.x, light.direction.y, light.direction.z);
			ImGui::Text("position: %.2f|%.2f|%.2f", light.position.x, light.position.y, light.position.z);
		}
		ImGui::End();
	}
}