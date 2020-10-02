#pragma once
#include "pch.h"

//着色器类
/*
使用前需要CreateShader()初始化着色器

使用SetShader()将上下文设置为当前着色器
*/
class ShaderBase {
public:
	ShaderBase() = default;
	~ShaderBase() = default;
	ShaderBase(const ShaderBase&) = delete;
	ShaderBase(ShaderBase&&) = default;

	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	//将渲染管道设置为当前所有着色器以及输入布局
	void SetShader(ID3D11DeviceContext* deviceContext) {
		deviceContext->IASetInputLayout(input_layout.Get());
		deviceContext->VSSetShader(VS_Shader.Get(), nullptr, 0);
		deviceContext->PSSetShader(PS_Shader.Get(), nullptr, 0);
		deviceContext->PSSetSamplers(0, 1, Samplers.GetAddressOf());
	}
	//分配顶点着色器
	void SetVS(ComPtr<ID3D11VertexShader>& _VS_Shader) {
		VS_Shader = std::move(_VS_Shader);
	}
	//将渲染管道设置为当前顶点着色器
	void SetVS(ID3D11DeviceContext* deviceContext) {
		deviceContext->VSSetShader(VS_Shader.Get(), nullptr, 0);
	}
	//获取顶点着色器
	ID3D11VertexShader* GetVS() {
		return VS_Shader.Get();
	}
	//分配像素着色器
	void SetPS(ComPtr<ID3D11PixelShader>& _PS_Shader) {
		PS_Shader = std::move(_PS_Shader);
	}
	//将渲染管道设置为当前像素着色器
	void SetPS(ID3D11DeviceContext* deviceContext) {
		deviceContext->PSSetShader(PS_Shader.Get(), nullptr, 0);
	}
	//获得像素着色器
	ID3D11PixelShader* GetPS() {
		return PS_Shader.Get();
	}
	//分配输入布局
	void SetInputLayout(ComPtr<ID3D11InputLayout>& layout) {
		input_layout = std::move(layout);
	}
	//将渲染管道设置为此着色器的输入布局
	void SetInputLayout(ID3D11DeviceContext* deviceContext) {
		deviceContext->IASetInputLayout(input_layout.Get());
	}
	//获得输入布局
	ID3D11InputLayout* GetInputLayout() {
		return input_layout.Get();
	}
	void SetSamplerState(ComPtr<ID3D11SamplerState> pSamplers) {
		Samplers = pSamplers;
	}
	void SetSamplerState(ID3D11DeviceContext* deviceContext) {
		deviceContext->PSSetSamplers(0, 1, Samplers.GetAddressOf());
	}
	//编译着色器并设置输入布局 自动增加后缀.cso & .hlsl
	//注意LPCSTR不是 wchar_t* 是 char*
	void CreateShader(ID3D11Device* device, const std::wstring& VS_file, const std::wstring& PS_file,
		const std::string& VS_entrypoint, const std::string& PS_entrypoint,
		const D3D11_INPUT_ELEMENT_DESC layout[], UINT layout_size){
		CreateShader(device, 
			(VS_file + L".cso").c_str(), (VS_file + L".hlsl").c_str(),
			(PS_file + L".cso").c_str(), (PS_file + L".hlsl").c_str(), 
			VS_entrypoint.c_str(), PS_entrypoint.c_str(), 
			layout, layout_size);
	}
	//编译着色器并设置输入布局
	//注意LPCSTR不是 wchar_t* 是 const char*
	void CreateShader(ID3D11Device* device,
		const WCHAR* VS_cso_file, const WCHAR* VS_hlsl_file, 
		const WCHAR* PS_cso_file, const WCHAR* PS_hlsl_file,
		LPCSTR VS_entrypoint, LPCSTR PS_entrypoint,
		const D3D11_INPUT_ELEMENT_DESC layout[], UINT layout_size) {

		ComPtr<ID3DBlob> blob = nullptr;
		//创建顶点着色器
		HR(CreateShaderFromFile(VS_cso_file, VS_hlsl_file, VS_entrypoint, "vs_5_0", blob.ReleaseAndGetAddressOf()));
		HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, VS_Shader.GetAddressOf()));

		// 创建并绑定顶点布局
		HR(device->CreateInputLayout(layout, layout_size,
			blob->GetBufferPointer(), blob->GetBufferSize(), input_layout.GetAddressOf()));

		// 创建像素着色器
		HR(CreateShaderFromFile(PS_cso_file, PS_hlsl_file, PS_entrypoint, "ps_5_0", blob.ReleaseAndGetAddressOf()));
		HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, PS_Shader.GetAddressOf()));
	}

	
private:
	ComPtr<ID3D11VertexShader> VS_Shader = nullptr;
	ComPtr<ID3D11PixelShader> PS_Shader = nullptr;
	ComPtr<ID3D11InputLayout> input_layout = nullptr;
	ComPtr<ID3D11SamplerState> Samplers = nullptr;
};
