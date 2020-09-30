#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
	: m_Transform(),
	m_IndexCount(),
	m_VertexStride()
{
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	//m_pTextureResource.push_back(texture);
	m_pTextureView = texture;
	throw;
}
void GameObject::SetTexture(ID3D11Device* d3dDevice, const WCHAR path_to_file[], int index) {
	if (index == m_pTextureResource.size())m_pTextureResource.push_back(nullptr);
	HR(DirectX::CreateWICTextureFromFile(d3dDevice, path_to_file, 
		reinterpret_cast<ID3D11Resource**>(m_pTextureResource[index].GetAddressOf()), nullptr));
}
void GameObject::SetTexture(
	ID3D11Device* d3dDevice, 
	ID3D11DeviceContext* d3dDeviceContext, 
	const std::vector<std::wstring>& fileNames,
	bool generateMips) {
	HR(DirectX::CreateTexture2DArrayFromFile(d3dDevice, d3dDeviceContext, fileNames, 
		m_pTextureArray.GetAddressOf(), m_pTextureView.GetAddressOf(), generateMips));
}
void GameObject::Draw(ID3D11DeviceContext* deviceContext)
{

	ID3D11Buffer* cbuffer;
	deviceContext->VSGetConstantBuffers(0, 1, &cbuffer);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(cbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(Transform), &m_Transform, sizeof(Transform));
	deviceContext->Unmap(cbuffer, 0);

	// 设置顶点/索引缓冲区
	UINT strides = m_VertexStride;
	UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShaderResources(0, 1, m_pTextureView.GetAddressOf());
	//deviceContext->PSSetShaderResources(0, 1, m_pTexture[0].GetAddressOf());
	//deviceContext->PSSetShaderResources(1, 1, m_pTexture[1].GetAddressOf());
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

