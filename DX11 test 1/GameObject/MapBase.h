#pragma once
#include "d3dApp.h"
/*
	除了画图什么也干不了的地图基类
	
	创建对象部分：
	->SetTexture()	设置纹理
	->SetBuffer()	设置缓冲区

	更新状态部分：
	->GetTransform()	获得待使用缓存引用
	->Draw()			画图

	使用 x - y 坐标
	(-1, 1) -> ( 1, 1)
	 ↑
	 ↑
	(-1,-1) -> ( 1,-1)

	绘制顺序：	1 2 
				0 3
				(0,1,2)->(2,3,0)
*/
class MapBase {
	struct VertexType {
		DirectX::XMFLOAT3 pos;	//x, y, depth
		DirectX::XMFLOAT2 tex;	//纹理
		float tex_id;			//纹理ID
		static constexpr D3D11_INPUT_ELEMENT_DESC inputLayout[3] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		VertexType() = default;
	};
	
	using IndexType = DWORD;

	enum class Unit : UINT {
		Empty = 0,	//普通道路
		Soft_wall,	//可破坏墙
		Hard_wall,	//不可破坏墙
	};
public:

	// 使用模板别名(C++11)简化类型名
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	MapBase() = default;
	~MapBase() = default;
	UINT get_width()const { return width; }
	UINT get_height()const { return height; }
	//返回顶点集/索引集对应位置的第一个索引
	UINT get_index(UINT _width, UINT _height) {
		return _width + _height * width;
	}
	void Init(ID3D11Device* device, ID3DBlob* blob);
	//修改地图尺寸 更新内存部分
	void resize(UINT _width, UINT _height);
	//加载地图纹理ID至内存
	void load(std::ifstream& in);
	//按队列更新顶点缓冲区
	void update_VerBuffer_by_queue(ID3D11DeviceContext* deviceContext);
	//整体修改顶点缓冲区
	void update_all_VerBuffer(ID3D11DeviceContext* deviceContext);
	void SetBuffer(ID3D11Device* device);
	//分配缓冲区
	void SetBuffer(ID3D11Device* device, const std::vector<VertexType>& vertexs, const std::vector<IndexType>& indexs);
	// 设置纹理
	void SetTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const std::vector<std::wstring>& fileNames, bool generateMips = false);


	// 绘制
	virtual void Draw(ID3D11DeviceContext* deviceContext);

	// 设置调试对象名
	// 若缓冲区被重新设置，调试对象名也需要被重新设置
	void SetDebugObjectName(const std::string& name);
private:
	//格子数量
	UINT width = 0;
	UINT height = 0;
	//显示区域
	DirectX::XMFLOAT2 range_width;	//[left, right]
	DirectX::XMFLOAT2 range_height;	//[down, up]
	//地图改变点的索引队列
	std::vector<UINT>vertex_changed;

	//CPU 控制端

	// 顶点集 每个格子使用4个顶点
	std::vector<VertexType> m_Vertexs;
	// 索引集 每个格子使用4个索引
	std::vector<IndexType> m_Indexs;

	//GPU 控制端
	ID3DBlob* m_pBlob;											// 着色器
	ComPtr<ID3D11InputLayout> m_pVertexLayout;					// 顶点输入布局
	ComPtr<ID3D11Texture2D>m_pTextureArray = nullptr;			// 纹理数组
	ComPtr<ID3D11ShaderResourceView> m_pTextureView = nullptr;	// 纹理资源视图
	ComPtr<ID3D11Buffer> m_pVertexBuffer = nullptr;				// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer = nullptr;				// 索引缓冲区
	const UINT m_VertexStride = sizeof(VertexType);				// 顶点字节大小
	UINT m_IndexCount = 0;										// 索引数目
};
