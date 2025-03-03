#pragma once

class DirectXBase;

class ModelBase {
private:
	// シングルトンパターンの適用
	static ModelBase* instance;

	// コンストラクタ、デストラクタの隠蔽
	ModelBase() = default;
	~ModelBase() = default;

	// コンストラクタ、コピー代入演算子の封印
	ModelBase(ModelBase&) = delete;
	ModelBase& operator=(ModelBase&) = delete;

public:
	// インスタンスの取得
	static ModelBase* GetInstance();

	// 終了処理
	void Finalize();

	// 初期化
	void Initialize(DirectXBase* directxBase);

	DirectXBase* GetDxBase() const { return directxBase_; }

private:
	DirectXBase* directxBase_;
};
