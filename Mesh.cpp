#include "Mesh.h"
#include "imgui/imgui.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include "LayoutCodex.h"
#include "Stencil.h"


namespace dx = DirectX;

//ModelException::ModelException(int line, const char* file, std::string note) noexcept
//	:
//	ExceptionBase(line, file),
//	note(std::move(note))
//{}
//
//const char* ModelException::what() const noexcept
//{
//	std::ostringstream oss;
//	oss << ExceptionBase::what() << std::endl
//		<< "[Note] " << GetNote();
//	whatBuffer = oss.str();
//	return whatBuffer.c_str();
//}
//
//const char* ModelException::GetType() const noexcept
//{
//	return "Model throws an Exception";
//}
//
//const std::string& ModelException::GetNote() const noexcept
//{
//	return note;
//}

// Mesh
Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept(!IS_DEBUG)
	:
	Drawable(gfx, mat, mesh, scale)
{}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}


void Mesh::Submit(dx::FXMMATRIX accumulatedTranform) const noexcept(!IS_DEBUG)
{
	dx::XMStoreFloat4x4(&transform, accumulatedTranform);
	Drawable::Submit();
}

//// Node
//Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noexcept(!IS_DEBUG)
//	:
//id(id),
//meshPtrs(std::move(meshPtrs)),
//name(name)
//{
//	dx::XMStoreFloat4x4(&transform, transform_in);
//	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
//}
//
//void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
//{
//	const auto built =
//		dx::XMLoadFloat4x4(&appliedTransform) *
//		dx::XMLoadFloat4x4(&transform) *
//		accumulatedTransform;
//	for (const auto pm : meshPtrs)
//	{
//		pm->Submit(frame, accumulatedTransform);
//	}
//	for (const auto& pc : childPtrs)
//	{
//		pc->Submit(frame, accumulatedTransform);
//	}
//}
//
//
//
//void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
//{
//	assert(pChild);
//	childPtrs.push_back(std::move(pChild));
//}
//
//void Node::ShowTree(Node*& pSelectedNode) const noexcept
//{
//	// if there is no selected node, set selectedId to an impossible value
//	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
//	// build up flags for current node
//	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
//		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
//		| ((childPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
//	// render this node
//	const auto expanded = ImGui::TreeNodeEx(
//		(void*)(intptr_t)GetId(), node_flags, name.c_str()
//	);
//	// processing for selecting node
//	if (ImGui::IsItemClicked())
//	{
//		pSelectedNode = const_cast<Node*>(this);
//	}
//	// recursive rendering of open node's children
//	if (expanded)
//	{
//		for (const auto& pChild : childPtrs)
//		{
//			pChild->ShowTree(pSelectedNode);
//		}
//		ImGui::TreePop();
//	}
//}
//
//void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
//{
//	dx::XMStoreFloat4x4(&appliedTransform, transform);
//}
//
//const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
//{
//	return appliedTransform;
//}
//
//int Node::GetId() const noexcept
//{
//	return id;
//}


// Model
//class ModelWindow // pImpl idiom, only defined in this .cpp
//{
//public:
//	void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
//	{
//	
//	}
//	void ApplyParameters() noexcept(!IS_DEBUG)
//	{
//
//	}
//private:
//	dx::XMMATRIX GetTransform() const noexcept(!IS_DEBUG)
//	{
//		assert(pSelectedNode != nullptr);
//		const auto& transform = transforms.at(pSelectedNode->GetId()).tranformParams;
//		return
//			dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
//			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
//	}
//	const Dcb::Buffer& GetMaterial() const noexcept(!IS_DEBUG)
//	{
//		assert(pSelectedNode != nullptr);
//		const auto& mat = transforms.at(pSelectedNode->GetId()).materialCbuf;
//		assert(mat);
//		return *mat;
//	}
//	bool TransformDirty() const noexcept(!IS_DEBUG)
//	{
//		return pSelectedNode && transforms.at(pSelectedNode->GetId()).transformParamsDirty;
//	}
//	void ResetTransformDirty() noexcept(!IS_DEBUG)
//	{
//		transforms.at(pSelectedNode->GetId()).transformParamsDirty = false;
//	}
//	bool MaterialDirty() const noexcept(!IS_DEBUG)
//	{
//		return pSelectedNode && transforms.at(pSelectedNode->GetId()).materialCbufDirty;
//	}
//	void ResetMaterialDirty() noexcept(!IS_DEBUG)
//	{
//		transforms.at(pSelectedNode->GetId()).materialCbufDirty = false;
//	}
//	bool IsDirty() const noexcept(!IS_DEBUG)
//	{
//		return TransformDirty() || MaterialDirty();
//	}
//private:
//	Node* pSelectedNode;
//	struct TransformParameters
//	{
//		float roll = 0.0f;
//		float pitch = 0.0f;
//		float yaw = 0.0f;
//		float x = 0.0f;
//		float y = 0.0f;
//		float z = 0.0f;
//	};
//	struct NodeData
//	{
//		TransformParameters tranformParams;
//		bool transformParamsDirty;
//		std::optional<Dcb::Buffer> materialCbuf;
//		bool materialCbufDirty;
//	};
//	std::unordered_map<int, NodeData> transforms;
//};
//
//
//Model::Model(Graphics& gfx, const std::string& pathString, const float scale)
//	:
//	pWindow(std::make_unique<ModelWindow>())
//{
//	Assimp::Importer imp;
//	const auto pScene = imp.ReadFile(pathString.c_str(),
//		aiProcess_Triangulate |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_ConvertToLeftHanded |
//		aiProcess_GenNormals |
//		aiProcess_CalcTangentSpace
//	);
//
//	if (pScene == nullptr)
//	{
//		throw ModelException(__LINE__, __FILE__, imp.GetErrorString());
//	}
//
//	for (size_t i = 0; i < pScene->mNumMeshes; i++)
//	{
//		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathString, scale));
//	}
//
//	int nextId = 0;
//	pRoot = ParseNode(nextId, *pScene->mRootNode);
//}
//
//void Model::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG)
//{
//	pWindow->ApplyParameters();
//	pRoot->Submit(frame, dx::XMMatrixIdentity());
//}
//
//void Model::ShowWindow(Graphics& gfx, const char* windowName) noexcept
//{
//	pWindow->Show(gfx, windowName, *pRoot);
//}
//
//void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept
//{
//	pRoot->SetAppliedTransform(tf);
//}
//
//Model::~Model() noexcept
//{}
//
//std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
//{
//	return {};
//	//using namespace std::string_literals;
//	//using hw3d::VertexLayout;
//	//using namespace Bind;
//
//	//const auto rootPath = path.parent_path().string() + "\\";
//
//	//std::vector<std::shared_ptr<Bindable>> bindablePtrs;
//
//	//bool hasSpecularMap = false;
//	//bool hasAlphaGloss = false;
//	//bool hasAlphaDiffuse = false;
//	//bool hasNormalMap = false;
//	//bool hasDiffuseMap = false;
//	//float shininess = 2.0f;
//	//dx::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
//	//dx::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };
//	//if (mesh.mMaterialIndex >= 0)
//	//{
//	//	auto& material = *pMaterials[mesh.mMaterialIndex];
//
//	//	aiString texFileName;
//
//	//	if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
//	//	{
//	//		auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
//	//		hasAlphaDiffuse = tex->HasAlpha();
//	//		bindablePtrs.push_back(std::move(tex));
//	//		hasDiffuseMap = true;
//	//	}
//	//	else
//	//	{
//	//		material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
//	//	}
//
//	//	if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
//	//	{
//	//		auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
//	//		hasAlphaGloss = tex->HasAlpha();
//	//		bindablePtrs.push_back(std::move(tex));
//	//		hasSpecularMap = true;
//	//	}
//	//	else
//	//	{
//	//		material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
//	//	}
//	//	if (!hasAlphaGloss)
//	//	{
//	//		material.Get(AI_MATKEY_SHININESS, shininess);
//	//	}
//
//	//	if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
//	//	{
//	//		auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2);
//	//		hasAlphaGloss = tex->HasAlpha();
//	//		bindablePtrs.push_back(std::move(tex));
//	//		hasNormalMap = true;
//	//	}
//
//	//	if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
//	//	{
//	//		bindablePtrs.push_back(Bind::Sampler::Resolve(gfx));
//	//	}
//	//}
//
//	//const auto meshTag = path.string() + "%" + mesh.mName.C_Str();
//
//	//if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
//	//{
//	//	hw3d::VertexBuffer vbuf(std::move(
//	//		VertexLayout{}
//	//		.Append(VertexLayout::Position3D)
//	//		.Append(VertexLayout::Normal)
//	//		.Append(VertexLayout::Tangent)
//	//		.Append(VertexLayout::Bitangent)
//	//		.Append(VertexLayout::Texture2D)
//	//	));
//
//	//	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//	//	{
//	//		vbuf.EmplaceBack(
//	//			dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
//	//			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//	//		);
//	//	}
//
//	//	std::vector<unsigned short> indices;
//	//	indices.reserve(mesh.mNumFaces * 3);
//	//	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//	//	{
//	//		const auto& face = mesh.mFaces[i];
//	//		assert(face.mNumIndices == 3);
//	//		indices.push_back(face.mIndices[0]);
//	//		indices.push_back(face.mIndices[1]);
//	//		indices.push_back(face.mIndices[2]);
//	//	}
//
//	//	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//	//	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//	//	auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
//	//	auto pvsbc = pvs->GetBytecode();
//	//	bindablePtrs.push_back(std::move(pvs));
//
//	//	bindablePtrs.push_back(PixelShader::Resolve(gfx,
//	//		hasAlphaDiffuse ? "PhongPSSpecNormMask.cso" : "PhongPSSpecNormalMap.cso"
//	//	));
//
//	//	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//	//	Dcb::RawLayout lay;
//	//	lay.Add<Dcb::Bool>("normalMapEnabled");
//	//	lay.Add<Dcb::Bool>("specularMapEnabled");
//	//	lay.Add<Dcb::Bool>("hasGlossMap");
//	//	lay.Add<Dcb::Float>("specularPower");
//	//	lay.Add<Dcb::Float3>("specularColor");
//	//	lay.Add<Dcb::Float>("specularMapWeight");
//
//	//	auto buf = Dcb::Buffer(std::move(lay));
//	//	buf["normalMapEnabled"] = true;
//	//	buf["specularMapEnabled"] = true;
//	//	buf["hasGlossMap"] = hasAlphaGloss;
//	//	buf["specularPower"] = shininess;
//	//	buf["specularColor"] = dx::XMFLOAT3{ 0.75f,0.75f,0.75f };
//	//	buf["specularMapWeight"] = 0.671f;
//
//	//	bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	//}
//	//else if (hasDiffuseMap && hasNormalMap)
//	//{
//	//	hw3d::VertexBuffer vbuf(std::move(
//	//		VertexLayout{}
//	//		.Append(VertexLayout::Position3D)
//	//		.Append(VertexLayout::Normal)
//	//		.Append(VertexLayout::Tangent)
//	//		.Append(VertexLayout::Bitangent)
//	//		.Append(VertexLayout::Texture2D)
//	//	));
//
//	//	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//	//	{
//	//		vbuf.EmplaceBack(
//	//			dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
//	//			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//	//		);
//	//	}
//
//	//	std::vector<unsigned short> indices;
//	//	indices.reserve(mesh.mNumFaces * 3);
//	//	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//	//	{
//	//		const auto& face = mesh.mFaces[i];
//	//		assert(face.mNumIndices == 3);
//	//		indices.push_back(face.mIndices[0]);
//	//		indices.push_back(face.mIndices[1]);
//	//		indices.push_back(face.mIndices[2]);
//	//	}
//
//	//	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//	//	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//	//	auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
//	//	auto pvsbc = pvs->GetBytecode();
//	//	bindablePtrs.push_back(std::move(pvs));
//
//	//	bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));
//
//	//	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//	//	Dcb::RawLayout layout;
//	//	layout.Add<Dcb::Float>("specularIntensity");
//	//	layout.Add<Dcb::Float>("specularPower");
//	//	layout.Add<Dcb::Bool>("normalMapEnabled");
//
//	//	auto cbuf = Dcb::Buffer(std::move(layout));
//	//	cbuf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
//	//	cbuf["specularPower"] = shininess;
//	//	cbuf["normalMapEnabled"] = true;
//
//	//	bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEX>(gfx, cbuf, 1u));
//	//}
//	//else if (hasDiffuseMap && !hasNormalMap && hasSpecularMap)
//	//{
//	//	hw3d::VertexBuffer vbuf(std::move(
//	//		VertexLayout{}
//	//		.Append(VertexLayout::Position3D)
//	//		.Append(VertexLayout::Normal)
//	//		.Append(VertexLayout::Texture2D)
//	//	));
//
//	//	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//	//	{
//	//		vbuf.EmplaceBack(
//	//			dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//	//			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//	//		);
//	//	}
//
//	//	std::vector<unsigned short> indices;
//	//	indices.reserve(mesh.mNumFaces * 3);
//	//	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//	//	{
//	//		const auto& face = mesh.mFaces[i];
//	//		assert(face.mNumIndices == 3);
//	//		indices.push_back(face.mIndices[0]);
//	//		indices.push_back(face.mIndices[1]);
//	//		indices.push_back(face.mIndices[2]);
//	//	}
//
//	//	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//	//	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//	//	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
//	//	auto pvsbc = pvs->GetBytecode();
//	//	bindablePtrs.push_back(std::move(pvs));
//
//	//	bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpec.cso"));
//
//	//	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//	//	Dcb::RawLayout lay;
//	//	lay.Add<Dcb::Float>("specularPower");
//	//	lay.Add<Dcb::Bool>("hasGloss");
//	//	lay.Add<Dcb::Float>("specularMapWeight");
//
//	//	auto buf = Dcb::Buffer(std::move(lay));
//	//	buf["specularPower"] = shininess;
//	//	buf["hasGloss"] = hasAlphaGloss;
//	//	buf["specularMapWeight"] = 1.0f;
//
//	//	bindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	//}
//	//else if (hasDiffuseMap)
//	//{
//	//	hw3d::VertexBuffer vbuf(std::move(
//	//		VertexLayout{}
//	//		.Append(VertexLayout::Position3D)
//	//		.Append(VertexLayout::Normal)
//	//		.Append(VertexLayout::Texture2D)
//	//	));
//
//	//	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//	//	{
//	//		vbuf.EmplaceBack(
//	//			dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//	//			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//	//		);
//	//	}
//
//	//	std::vector<unsigned short> indices;
//	//	indices.reserve(mesh.mNumFaces * 3);
//	//	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//	//	{
//	//		const auto& face = mesh.mFaces[i];
//	//		assert(face.mNumIndices == 3);
//	//		indices.push_back(face.mIndices[0]);
//	//		indices.push_back(face.mIndices[1]);
//	//		indices.push_back(face.mIndices[2]);
//	//	}
//
//	//	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//	//	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//	//	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
//	//	auto pvsbc = pvs->GetBytecode();
//	//	bindablePtrs.push_back(std::move(pvs));
//
//	//	bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
//
//	//	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//	//	Dcb::RawLayout lay;
//	//	lay.Add<Dcb::Float>("specularIntensity");
//	//	lay.Add<Dcb::Float>("specularPower");
//
//	//	auto buf = Dcb::Buffer(std::move(lay));
//	//	buf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
//	//	buf["specularPower"] = shininess;
//	//	buf["specularMapWeight"] = 1.0f;
//
//	//	bindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	//}
//	//else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
//	//{
//	//	hw3d::VertexBuffer vbuf(std::move(
//	//		VertexLayout{}
//	//		.Append(VertexLayout::Position3D)
//	//		.Append(VertexLayout::Normal)
//	//	));
//
//	//	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//	//	{
//	//		vbuf.EmplaceBack(
//	//			dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//	//			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
//	//		);
//	//	}
//
//	//	std::vector<unsigned short> indices;
//	//	indices.reserve(mesh.mNumFaces * 3);
//	//	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//	//	{
//	//		const auto& face = mesh.mFaces[i];
//	//		assert(face.mNumIndices == 3);
//	//		indices.push_back(face.mIndices[0]);
//	//		indices.push_back(face.mIndices[1]);
//	//		indices.push_back(face.mIndices[2]);
//	//	}
//
//	//	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//	//	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//	//	auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
//	//	auto pvsbc = pvs->GetBytecode();
//	//	bindablePtrs.push_back(std::move(pvs));
//
//	//	bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));
//
//	//	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//	//	Dcb::RawLayout lay;
//	//	lay.Add<Dcb::Float4>("materialColor");
//	//	lay.Add<Dcb::Float4>("specularColor");
//	//	lay.Add<Dcb::Float>("specularPower");
//
//	//	auto buf = Dcb::Buffer(std::move(lay));
//	//	buf["specularPower"] = shininess;
//	//	buf["specularColor"] = specularColor;
//	//	buf["materialColor"] = diffuseColor;
//
//	//	bindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	//}
//	//else
//	//{
//	//	throw std::runtime_error("terrible combination of textures in material smh");
//	//}
//
//	//// anything with alpha diffuse is 2-sided IN SPONZA, need a better way
//	//// of signalling 2-sidedness to be more general in the future
//	//bindablePtrs.push_back(Rasterizer::Resolve(gfx, hasAlphaDiffuse));
//
//	//bindablePtrs.push_back(Blender::Resolve(gfx, false));
//
//
//	//bindablePtrs.push_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Off));
//
//
//	//return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
//}
//
//std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
//{
//	namespace dx = DirectX;
//	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
//		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
//	));
//
//	std::vector<Mesh*> curMeshPtrs;
//	curMeshPtrs.reserve(node.mNumMeshes);
//	for (size_t i = 0; i < node.mNumMeshes; i++)
//	{
//		const auto meshIdx = node.mMeshes[i];
//		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
//	}
//
//	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
//	for (size_t i = 0; i < node.mNumChildren; i++)
//	{
//		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
//	}
//
//	return pNode;
//}