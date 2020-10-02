#include "pch.h"
#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <array>
#include <vector>
using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{

}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// 务必先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(m_pd3dDevice.Get());
	// 编译着色器
	if (!InitEffect()) {
		throw("着色器编译错误");
	};
	// 加载资源
	if (!InitResource()) return false;
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
	static int i = 0;
	i++;

#if 1
	sample->GetTransform().place = {0, 0, 0, 0};
#endif
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
#if 0
	//for(int i = 0; i < 2048; i++)
	sample->Draw(m_pd3dImmediateContext.Get());
#elif 1
	map->Draw(m_pd3dImmediateContext.Get());
#endif
	HR(m_pSwapChain->Present(0, 0));
}
//初始化着色器
bool GameApp::InitEffect()
{
	shader2D = std::make_unique<ShaderBase>();
	//编译着色器并设置输入布局
	shader2D->CreateShader(m_pd3dDevice.Get(), LR"(HLSL\VS_2D)", LR"(HLSL\PS_2D)", "VS_2D", "PS_2D", 
		VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout));
	shader2D->SetSamplerState(RenderStates::SSLinearWrap);

	MapShader = std::make_unique<ShaderBase>();
	MapShader->CreateShader(m_pd3dDevice.Get(), LR"(HLSL\MapVS)", LR"(HLSL\MapPS)", "VS", "PS",
		MapBase::VertexType::inputLayout, ARRAYSIZE(MapBase::VertexType::inputLayout));
	MapShader->SetSamplerState(RenderStates::SSLinearWrap);

	return true;
}

bool GameApp::InitResource()
{
	// 设置图元类型为每3个点为一个三角形
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//设置颜色合成为Alpha透明混合渲染
	m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);

	sample = std::make_unique<GameObject>();
	//载入纹理
	sample->SetTexture(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(), 
		{ LR"(Resource\test1.png)" , LR"(Resource\test2.png)" }, false);

	auto meshData = Geometry::Create2DShow(DirectX::XMFLOAT2(0, 0), DirectX::XMFLOAT2(1, 1));
	sample->SetBuffer(m_pd3dDevice.Get(), meshData);
	sample->CreateConstantBuffer(m_pd3dDevice.Get());

#if 0
	shader2D->SetShader(m_pd3dImmediateContext.Get());
#elif 1
	map = std::make_unique<MapBase>();
	map->Init(MapShader.get());
	map->resize(1000, 1000);
	map->SetBuffer(m_pd3dDevice.Get());
	map->SetTexture(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(),
		{ LR"(Resource\test1.png)" , LR"(Resource\test2.png)" },false);
#endif
	return true;
}


template<class VertexType>
bool GameApp::ResetMesh(const Geometry::MeshData<VertexType>& meshData)
{
	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();



	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexType);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexType);			// 跨越字节数
	UINT offset = 0;							// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);



	// 设置索引缓冲区描述
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * m_IndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// 输入装配阶段的索引缓冲区设置
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);



	// 设置调试对象名
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

	return true;
}
