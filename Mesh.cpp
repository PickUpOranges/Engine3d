#include "Mesh.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include <unordered_map>

namespace dx = DirectX;
using namespace Bind;
// Mesh
Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

	for (auto& pb : bindPtrs)
	{
		if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
			pb.release();
		}
		else
		{
			AddBind(std::move(pb));
		}
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTransform);
	Drawable::Draw(gfx);
}
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}

void Mesh::Update(float dt) noexcept
{
}


// Node
Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noexcept(!IS_DEBUG)
	:
	id(id),
	meshPtrs(std::move(meshPtrs)),
	name(name)
{
	dx::XMStoreFloat4x4(&baseTransform, transform);
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const auto built =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&baseTransform) *
		accumulatedTransform/* acc: {Parent}->{Root}->{World} */;

	for (const auto pm : meshPtrs)
	{
		pm->Draw(gfx, built);
	}
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, built);
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

void Node::ShowTree(std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept
{

	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedIndex.value_or(-1)) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);

	

	// if tree node expanded, recursively render all children
	if (ImGui::TreeNodeEx((void*)(intptr_t)GetId(), node_flags, name.c_str()))
	{
		// If current node is selected, then update the seletedidx to current node.
		if (ImGui::IsItemClicked())
		{
			selectedIndex = GetId();
			pSelectedNode = const_cast<Node*>(this);
		}
		
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(selectedIndex, pSelectedNode);
		}
		ImGui::TreePop();
	}
}


int Node::GetId() const noexcept
{
	return id;
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&appliedTransform, transform);
}



// Model
class ModelWindow // pImpl idiom, only defined in this .cpp
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(selectedIndex, pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				auto& transform = transforms[*selectedIndex];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = transforms.at(*selectedIndex);
		return
			dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}

	Node* GetSelectedNode() const noexcept
	{
		return pSelectedNode;
	}
private:

	Node* pSelectedNode;
	struct TransformParameters
	{
		// Note: rotation in radius.
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::optional<int> selectedIndex;
	std::unordered_map<int, TransformParameters> transforms;
};

Model::Model(Graphics& gfx, const std::string fileName)
	:
	pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(fileName.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices
	);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void Model::Draw(Graphics& gfx) const
{
	if (auto node = pWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(pWindow->GetTransform());
	}
	pRoot->Draw(gfx, dx::XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	pWindow->Show(windowName, *pRoot);
}

Model::~Model() noexcept
{
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using hw3d::VertexLayout;

	hw3d::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal)
		.Append(VertexLayout::Texture2D)
	));


	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);
	}

	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	std::vector<std::unique_ptr<Bindable>> bindablePtrs;

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		using namespace std::string_literals;
		const auto base = "Models\\nano_textured\\"s;
		aiString texFileName;
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str())));
		
		material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName);
		bindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1));
		bindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));
	}


	bindablePtrs.push_back(std::make_unique<VertexBuffer>(gfx, vbuf));

	bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));

	auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	bindablePtrs.push_back(std::move(pvs));

	bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

	bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	struct PSMaterialConstant
	{
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[2];
	} pmc;
	bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}
std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}