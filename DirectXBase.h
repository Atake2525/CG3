#pragma once
#include "WinApp.h"
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <array>
#include <dxcapi.h>
#include <string>
#include <cstdint>
#include <chrono>
#include <thread>
#include "externels/DirectXTex/DirectXTex.h"


class DirectXBase {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp);

	// 描画前処理
	void PreDraw();

	// 描画後処理
	void PostDraw();

	// 終了処理
	void Finalize();

	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t kMaxSRVCount;

	/// <summary>
	/// SRVの指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	
	/// <summary>
	/// SRVの指定番号のGPUデスクリプタハンドル
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);
	
	// DSVとRTVも作る

	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource;

	// getter
	Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const { return device.Get(); }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return commandList.Get(); }
	// SwapChainからResourceを引っ張ってくる
	//Microsoft::WRL::ComPtr<ID3D12Resource> GetSwapChainResources(UINT i) const { return swapChainResources[i].Get(); };

	/// <summary>
	/// ディスクリプターヒープの作成関数
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	// シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);

	// DirectX12のTextureResourceを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

private:


	// DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, int32_t width, int32_t height);

	// FPS固定初期化
	void InitializeFixFPS();
	// FPS固定更新
	void UpdateFixFPS();

	// 起動時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

	/// <summary>
	/// デバイス初期化
	/// </summary>
	void InitializeDevice();
	/// <summary>
	/// スワップチェイン初期化
	/// </summary>
	void CreateSwapChain();
	/// <summary>
	/// 深度バッファ初期化
	/// </summary>
	void CreateDepthBuffer();
	/// <summary>
	/// コマンド関連の初期化
	/// </summary>
	void InitializeCommands();
	/// <summary>
	/// 各種ディスクリプターヒープの作成
	/// </summary>
	void MakeDescriptorHeap();
	/// <summary>
	/// レンダーターゲットビューの初期化
	/// </summary>
	void InitializeRenderTargetView();
	/// <summary>
	/// 深度ステンシルビューの初期化
	/// </summary>
	void InitializeDepthStenCilView();
	/// <summary>
	/// フェンスの初期化
	/// </summary>
	void InitializeFence();
	/// <summary>
	///  ビューポート矩形の初期化
	/// </summary>
	void InitializeViewPortRect();
	/// <summary>
	/// シザー矩形の初期化
	/// </summary>
	void InitializeScissorRect();
	/// <summary>
	/// DXCコンパイラの生成
	/// </summary>
	void CreateDXCCompiler();
	/// <summary>
	/// ImGuiの初期化
	/// </summary>
	void InitializeImgui();
	/// <summary>
	/// 指定番号のCPUデスクリプタハンドルを取得する
	/// </summary>
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
	/// <summary>
	/// 指定番号のGPUデスクリプタハンドルを取得する
	/// </summary>
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	// ポインタ
	WinApp* winApp_ = nullptr;

	HRESULT hr; 
	// DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;
	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	// コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	// コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	// スワップチェイン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	// 深度バッファ
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// レンダーターゲットビュー
	// スワップチェイン
	// SwapChainからResourceを引っ張ってくる
	//std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//// フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;
	// ビューポート矩形
	D3D12_VIEWPORT viewPort{};
	// シザー矩形
	D3D12_RECT scissorRect{};
	// dxcCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler = nullptr;
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//// DepthStencilStateの設定
	
	//
	//// バックバッファの番号
	UINT backBufferIndex;
	// リソースバリア
	D3D12_RESOURCE_BARRIER barrier{};
	// dsvHandle
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	//


	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	// RTV様のヒープでディスクリプタの数は2。RTVはShader内で触るものではないので、ShaderVisibleはfalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;

	// SRV様のヒープでディスクリプタの数は128。SRVはShader内で触るものなので、shaderVisibleはtrue
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;

	// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはFalse
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
};