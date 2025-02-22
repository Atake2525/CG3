#pragma once

class DirectXBase;

class ModelBase {
public:
	// 初期化
	void Initialize(DirectXBase* directxBase);

	DirectXBase* GetDxBase() const { return directxBase_; }

private:
	DirectXBase* directxBase_;
};
