#pragma once
#include "d3dApp.h"
#include "Geometry.h"
#include "RenderStates.h"
#include "WICTextureLoader.h"
#include "DXTrace.h"
#include "ShaderBase.h"
#include "GameObject.h"
#include "MapBase.h"

class GameApp : public D3DApp
{
public:
	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		static constexpr D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
	};
	struct VSConstantBuffer	//常量缓冲功能区必须16字节对齐
	{
		DirectX::XMFLOAT4 pos = {0,0,0,0};
	};


	struct VertexPosTex
	{
		VertexPosTex() = default;

		VertexPosTex(const VertexPosTex&) = default;
		VertexPosTex& operator=(const VertexPosTex&) = default;

		VertexPosTex(VertexPosTex&&) = default;
		VertexPosTex& operator=(VertexPosTex&&) = default;

		constexpr VertexPosTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _tex) :
			pos(_pos), tex(_tex) {}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		static constexpr D3D11_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
	};



public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();


private:
	//初始化着色器
	bool InitEffect();
	//初始化资源
	bool InitResource();
	//根据对象自动初始化资源
	template<class VertexType>
	bool ResetMesh(const Geometry::MeshData<VertexType>& meshData);

private:
	std::unique_ptr<MapBase> map;
	std::unique_ptr<GameObject> sample;

	ComPtr<ID3D11InputLayout> m_pVertexLayout2D;		// 用于2D的顶点输入布局

	ComPtr<ID3D11Buffer> m_pVertexBuffer;				// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;				// 索引缓冲区
	ComPtr<ID3D11Buffer> m_pConstantBuffers;			// 常量缓冲区

	UINT m_IndexCount = 0;									// 绘制物体的索引数组大小

	ShaderBase shader2D;
	ComPtr<ID3D11VertexShader> m_pVertexShader2D;		// 用于2D的顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;			// 用于2D的像素着色器


	//ComPtr<ID3D11ShaderResourceView> m_test_resource;

	VSConstantBuffer m_VSConstantBuffer;		// 用于修改用于VS的GPU常量缓冲区的变量

};
