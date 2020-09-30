#include "pch.h"
#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include <array>
#include <vector>
using namespace DirectX;
#if 0
const D3D11_INPUT_ELEMENT_DESC GameApp::VertexPosColor::inputLayout[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};
#endif
GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	std::vector<std::vector<int>>v;
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
	if (!InitEffect())
		return false;
	// 加载资源
	if (!InitResource())
		return false;

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

#if 0
	ZeroMemory(&m_VSConstantBuffer, sizeof(m_VSConstantBuffer));
	//m_VSConstantBuffer.pos.x = (FLOAT)rand() / RAND_MAX;
	if (i % 2)m_VSConstantBuffer.pos = { 0.2,-0.1,0.1,0 };
	else m_VSConstantBuffer.pos = { -0.2,0,-0.1,0 };
	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(VSConstantBuffer), &m_VSConstantBuffer, sizeof(VSConstantBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers.Get(), 0);

	
	std::array<float, 4> black;
	if (i % 20000 < 10000)black = { 0.0f, 0.0f, 0.0f, 1.0f };
	else black = { 1.0f, 1.0f, 1.0f, 1.0f };	// RGBA = (0,0,0,255)
	if(i%2) m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black.data());
#elif 0
	//m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_test_resource.GetAddressOf());
#elif 1
	sample->GetTransform().place = {0, 0, 0, 0};
#endif
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);
#if 0
	//static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// RGBA = (0,0,0,255)
	//m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), black);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制三角形
	m_pd3dImmediateContext->Draw(3, 0);

	UpdateScene(0);
	m_pd3dImmediateContext->Draw(3, 0);

	HR(m_pSwapChain->Present(0, 0));
#elif 0
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制几何模型
	m_pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
#elif 1
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for(int i = 0; i < 256; i++)
	sample->Draw(m_pd3dImmediateContext.Get());
	
#endif
	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitEffect()
{
	ComPtr<ID3DBlob> blob;

	//3D 部分

	// 创建顶点着色器
	HR(CreateShaderFromFile(LR"(HLSL\Triangle_VS.cso)", LR"(HLSL\Triangle_VS.hlsl)", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(LR"(HLSL\Triangle_PS.cso)", LR"(HLSL\Triangle_PS.hlsl)", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	//2D部分

	// 创建顶点着色器(2D)
	HR(CreateShaderFromFile(LR"(HLSL\VS_2D.cso)", LR"(HLSL\VS_2D.hlsl)", "VS_2D", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader2D.GetAddressOf()));
	// 创建顶点布局(2D)
	HR(m_pd3dDevice->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout2D.GetAddressOf()));

	// 创建像素着色器(2D)
	HR(CreateShaderFromFile(LR"(HLSL\PS_2D.cso)", LR"(HLSL\PS_2D.hlsl)", "PS_2D", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader2D.GetAddressOf()));

	return true;
}

bool GameApp::InitResource()
{
#if 1
	//常量缓冲区部分
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(VSConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建用于VS和PS的常量缓冲区
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers.GetAddressOf()));
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers.GetAddressOf());
#endif


	// 设置三角形顶点
	VertexPosColor vertices[] =
	{
		{ XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f) },
		{ XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f) },
		{ XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f) }
	};
	// 设置顶点缓冲区描述.
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.ByteWidth = sizeof vertices;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));
	

	// ******************
	// 给渲染管线各个阶段绑定好所需资源
	//

	// 输入装配阶段的顶点缓冲区设置
	const UINT stride[] = { sizeof(VertexPosColor) };	// 跨越字节数
	const UINT offset[] = { 0 };						// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), stride, offset);


	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	m_pd3dImmediateContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);

	// ******************
	// 设置调试对象名
	//
	D3D11SetDebugObjectName(m_pVertexLayout.Get(), "VertexPosColorLayout");
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pVertexShader.Get(), "Trangle_VS");
	D3D11SetDebugObjectName(m_pVertexShader2D.Get(), "VS_2D");

	D3D11SetDebugObjectName(m_pPixelShader.Get(), "Trangle_PS");
	D3D11SetDebugObjectName(m_pPixelShader2D.Get(), "PS_2D");

	D3D11SetDebugObjectName(m_pConstantBuffers.Get(), "ConstantBuffers");



	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout2D.Get());
#if 0
	WCHAR strFile[40] = L"test.png";
	HR(CreateWICTextureFromFile(m_pd3dDevice.Get(), strFile, nullptr, m_test_resource.GetAddressOf()));
	
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout2D.Get());
	auto meshData = Geometry::Create2DShow();
	ResetMesh(meshData);
#else
	sample = std::make_unique<GameObject>();
#if 0
	sample->SetTexture(m_pd3dDevice.Get(), L"test1.png", 0);
	sample->SetTexture(m_pd3dDevice.Get(), L"test2.png", 1);
#elif 1
	sample->SetTexture(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(), 
		{ LR"(Resource\test1.png)" , LR"(Resource\test2.png)" }, false);
#endif
	auto meshData = Geometry::Create2DShow();
	sample->SetBuffer(m_pd3dDevice.Get(), meshData);
	//ResetMesh(meshData);
#endif

	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout2D.Get());
	
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader2D.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader2D.Get(), nullptr, 0);
	//m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_test_resource.GetAddressOf());
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
