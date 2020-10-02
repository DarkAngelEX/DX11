#include "pch.h"
#include "GameObject.h"


void GameObject::CreateConstantBuffer(ID3D11Device* device) {
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(Transform);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建常量缓冲区，不使用初始数据
	HR(device->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.ReleaseAndGetAddressOf()));
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTextureView = texture;
	throw;
}
void GameObject::SetTexture(ID3D11Device* device, const WCHAR path_to_file[], int index) 
{
	if (index == m_pTextureResource.size())m_pTextureResource.push_back(nullptr);
	HR(DirectX::CreateWICTextureFromFile(device, path_to_file,
		reinterpret_cast<ID3D11Resource**>(m_pTextureResource[index].GetAddressOf()), nullptr));
}
void GameObject::SetTexture(
	ID3D11Device* device, 
	ID3D11DeviceContext* deviceContext, 
	const std::vector<std::wstring>& fileNames,
	bool generateMips) 
{
	HR(DirectX::CreateTexture2DArrayFromFile(device, deviceContext, fileNames,
		m_pTextureArray.GetAddressOf(), m_pTextureView.GetAddressOf(), generateMips));
}
void GameObject::Draw(ID3D11DeviceContext* deviceContext)
{
#if 1
	deviceContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	//ID3D11Buffer* cbuffer;
	//deviceContext->VSGetConstantBuffers(0, 1, &cbuffer);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(Transform), &m_Transform, sizeof(Transform));
	deviceContext->Unmap(m_pConstantBuffer.Get(), 0);
#endif
	// 设置顶点/索引缓冲区
	UINT strides = m_VertexStride;
	UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShaderResources(0, 1, m_pTextureView.GetAddressOf());
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), name + ".VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), name + ".IndexBuffer");
#else
	UNREFERENCED_PARAMETER(name);
#endif
}

