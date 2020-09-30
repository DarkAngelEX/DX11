#pragma once
#include "d3dApp.h"
#include "Geometry.h"
#include "RenderStates.h"
#include "WICTextureLoader.h"
#include "DXTrace.h"
#include "GameObject.h"

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
	struct VSConstantBuffer	//�������幦��������16�ֽڶ���
	{
		DirectX::XMFLOAT4 pos;
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
	bool InitEffect();
	bool InitResource();

	template<class VertexType>
	bool ResetMesh(const Geometry::MeshData<VertexType>& meshData);

private:
	std::unique_ptr<GameObject> sample;

	ComPtr<ID3D11InputLayout> m_pVertexLayout2D;		// ����2D�Ķ������벼��
	ComPtr<ID3D11InputLayout> m_pVertexLayout;			// �������벼��
	ComPtr<ID3D11InputLayout> m_pConstantLayout;		// �������벼��

	ComPtr<ID3D11Buffer> m_pVertexBuffer;				// ���㻺����
	ComPtr<ID3D11Buffer> m_pIndexBuffer;				// ����������
	ComPtr<ID3D11Buffer> m_pConstantBuffers;			// ����������

	UINT m_IndexCount;									// ������������������С

	ComPtr<ID3D11VertexShader> m_pVertexShader;			// ������ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader;			// ������ɫ��
	ComPtr<ID3D11VertexShader> m_pVertexShader2D;		// ����2D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader> m_pPixelShader2D;			// ����2D��������ɫ��

	ComPtr<ID3D11ShaderResourceView> m_test_resource;

	VSConstantBuffer m_VSConstantBuffer;		// �����޸�����VS��GPU�����������ı���

};
