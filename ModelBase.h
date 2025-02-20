#pragma once

class DirectXBase;

class ModelBase {
//private:
//	// コンストラクタ、デストラクタの隠蔽
//	ModelBase() = default;
//	~ModelBase() = default;
//	// コピーコンストラクタ、コピー代入演算子の封印
//	ModelBase(ModelBase&) = delete;
//	ModelBase& operator=(ModelBase&) = delete;

public:
	// 初期化
	void Initialize(DirectXBase* directxBase);

	DirectXBase* GetDxBase() const { return directxBase_; }

private:
	DirectXBase* directxBase_;
};
