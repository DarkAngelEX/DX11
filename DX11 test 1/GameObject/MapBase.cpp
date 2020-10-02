#include "pch.h"
#include "MapBase.h"


void MapBase::Init(ID3D11Device* device, ID3DBlob* blob) {
	m_pBlob = blob;
	device->CreateInputLayout(VertexType::inputLayout, ARRAYSIZE(VertexType::inputLayout),
		m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), m_pVertexLayout.GetAddressOf());
}

//修改地图尺寸 更新内存部分
void MapBase::resize(UINT _width, UINT _height) {
	width = _width;
	height = _height;
	m_Vertexs.resize(_width * _height * 4);
	m_Indexs.resize(_width * _height * 4);
	for (UINT h = 0; h < height; h++) {
		for (UINT w = 0; w < width; w++) {
			auto fun = [this](UINT w, UINT h, float depth = 0.99)->DirectX::XMFLOAT3 {
				return DirectX::XMFLOAT3(range_width.x + (range_width.y - range_width.x) * w / width,
					range_height.x + (range_height.y - range_height.x) * h / height, 0.99);
			};
			/*
			1 2
			0 3
			*/
			auto ver_index = get_index(w, h) * 4;
			m_Vertexs[ver_index + 0] = { fun(w    , h    ), DirectX::XMFLOAT2(0, 1), 0 };
			m_Vertexs[ver_index + 1] = { fun(w + 1, h    ), DirectX::XMFLOAT2(0, 0), 0 };
			m_Vertexs[ver_index + 2] = { fun(w + 1, h + 1), DirectX::XMFLOAT2(1, 0), 0 };
			m_Vertexs[ver_index + 3] = { fun(w    , h + 1), DirectX::XMFLOAT2(1, 1), 0 };
			
			auto ind_index = get_index(w, h) * 6;
			m_Indexs[ind_index + 0] = 0;
			m_Indexs[ind_index + 1] = 1;
			m_Indexs[ind_index + 2] = 2;
			m_Indexs[ind_index + 3] = 2;
			m_Indexs[ind_index + 4] = 3;
			m_Indexs[ind_index + 5] = 0;
		}
	}
}

//加载地图纹理ID至内存
void MapBase::load(std::ifstream& in) {
	UINT _width = 0, _height = 0;
	in >> _width >> _height;

	if (_width != width || _height != height) {
		MapBase::resize(_width, _height);
	}

	for (UINT h = 0; h < height; h++) {
		for (UINT w = 0; w < width; w++) {
			UINT id; in >> id; 
			auto ver_index = get_index(w, h) * 4;
			for(int i = 0; i < 4; i++) m_Vertexs[ver_index + i].tex_id = id;
		}
	}
}

void MapBase::update_VerBuffer_by_queue(ID3D11DeviceContext* deviceContext) {
	if (vertex_changed.empty())return;
	std::sort(vertex_changed.begin(), vertex_changed.end());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//memcpy_s(mappedData.pData, sizeof(Transform), &m_Transform, sizeof(Transform));
	for (auto index : vertex_changed) {
		for (UINT i = 0; i < 4; i++) {
#if defined(_DEBUG) || defined(DEBUG)
			assert(i == 0 || m_Vertexs[index * 4 + i].tex_id == m_Vertexs[index * 4 + i - 1].tex_id);
#endif
			reinterpret_cast<VertexType*>(mappedData.pData)[index * 4 + i].tex_id = m_Vertexs[index * 4 + i].tex_id;
		}
	}
	deviceContext->Unmap(m_pVertexBuffer.Get(), 0);
}
void MapBase::update_all_VerBuffer(ID3D11DeviceContext* deviceContext) {
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(m_pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	//memcpy_s(mappedData.pData, sizeof(Transform), &m_Transform, sizeof(Transform));
	for (UINT index = 0; index < m_Vertexs.size(); index += 4) {
		for (UINT i = 0; i < 4; i++) {
#if defined(_DEBUG) || defined(DEBUG)
			assert(i == 0 || m_Vertexs[index * 4 + i].tex_id == m_Vertexs[index * 4 + i - 1].tex_id);
#endif
			reinterpret_cast<VertexType*>(mappedData.pData)[index * 4 + i].tex_id = m_Vertexs[index * 4 + i].tex_id;
		}
	}
	deviceContext->Unmap(m_pVertexBuffer.Get(), 0);
}
void MapBase::SetBuffer(ID3D11Device* device) {
	SetBuffer(device, m_Vertexs, m_Indexs);
}
void MapBase::SetBuffer(ID3D11Device* device, const std::vector<VertexType>& vertexs, const std::vector<IndexType>& indexs) {
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 检查D3D设备
	if (device == nullptr) throw;
	m_Vertexs = vertexs;
	m_Indexs = indexs;

	// 设置顶点缓冲区描述
	//m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.Usage = D3D11_USAGE_DYNAMIC;	//用于修改纹理标号
	vbd.ByteWidth = (UINT)vertexs.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_CPU_ACCESS_WRITE;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertexs.data();
	device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf());

	// 设置索引缓冲区描述
	m_IndexCount = (UINT)indexs.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indexs.data();
	device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf());
}

// 设置纹理至缓冲区
void MapBase::SetTexture(
	ID3D11Device* device, 
	ID3D11DeviceContext* deviceContext,
	const std::vector<std::wstring>& fileNames, 
	bool generateMips) 
{
	HR(DirectX::CreateTexture2DArrayFromFile(device, deviceContext, fileNames,
		m_pTextureArray.GetAddressOf(), m_pTextureView.GetAddressOf(), generateMips));
}


// 绘制
void MapBase::Draw(ID3D11DeviceContext* deviceContext) {
	update_VerBuffer_by_queue(deviceContext);
	UINT strides = m_VertexStride;
	UINT offsets = 0;

	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->PSSetShaderResources(0, 1, m_pTextureView.GetAddressOf());
	deviceContext->IASetInputLayout(m_pVertexLayout.Get());
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

// 设置调试对象名
// 若缓冲区被重新设置，调试对象名也需要被重新设置
void MapBase::SetDebugObjectName(const std::string& name) {}