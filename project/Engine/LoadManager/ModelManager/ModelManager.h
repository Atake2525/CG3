#include <map>
#include <string>
#include <memory>

#pragma once

class Model;
class ModelBase;
class DirectXBase;

class ModelManager {
private:
	// シングルトンパターンを適用
	static ModelManager* instance;

	// コンストラクタ、デストラクタの隠蔽
	ModelManager() = default;
	~ModelManager() = default;
	// コピーコンストラクタ、コピー代入演算子の封印
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = delete;

public:

	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	// 終了
	void Finalize();

	// 初期化
	void Initialize(DirectXBase* directxBase);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"> : ディレクトリ(元ファイル)のパス</param>
	/// <param name="filePath"> : モデルファイルのパス</param>
	/// <param name="enableLighting"> : ライティングを適用するかどうか</param>
	/// enableLightingは何も入力しなければfalse
	void LoadModel(const std::string& directoryPath, const std::string& filePath, const bool& enableLighting = false);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& filePath);

private:
	// モデレータ
	std::map<std::string, std::unique_ptr<Model>> models;

	ModelBase* modelBase = nullptr;
};
