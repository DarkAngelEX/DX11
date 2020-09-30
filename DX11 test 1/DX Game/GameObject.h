#pragma once
#include "d3dApp.h"
#include "Geometry.h"

// һ��������С����Ϸ������
class GameObject
{
	struct Transform {
		DirectX::XMFLOAT4 place;	//<dx, dy, d_depth, d_size> add<0,1,2> mul<3>
	};
public:

	// ��ȡ����任
	Transform& GetTransform() { return m_Transform; }
	// ��ȡ����任
	const Transform& GetTransform() const { return m_Transform; }

	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	GameObject();

	// ���û�����
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	// ��������
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetTexture(ID3D11Device* d3dDevice, const WCHAR path_to_file[], int index);
	void SetTexture(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dDeviceContext,
		const std::vector<std::wstring>& fileNames, bool generateMips = false);


	// ����
	void Draw(ID3D11DeviceContext* deviceContext);

	// ���õ��Զ�����
	// �����������������ã����Զ�����Ҳ��Ҫ����������
	void SetDebugObjectName(const std::string& name);
private:
	Transform m_Transform;								// ����任��Ϣ

	
	std::vector<ComPtr<ID3D11Texture2D>> m_pTextureResource;	// ������Դ
	ComPtr<ID3D11Texture2D>m_pTextureArray;						//��������
	ComPtr<ID3D11ShaderResourceView> m_pTextureView;			// ������Դ��ͼ
	ComPtr<ID3D11Buffer> m_pVertexBuffer;				// ���㻺����
	ComPtr<ID3D11Buffer> m_pIndexBuffer;				// ����������
	UINT m_VertexStride;								// �����ֽڴ�С
	UINT m_IndexCount;								    // ������Ŀ
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	// �ͷž���Դ
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// ���D3D�豸
	if (device == nullptr)
		return;

	// ���ö��㻺��������
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf());

	// ������������������
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	InitData.pSysMem = meshData.indexVec.data();
	device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf());

}
