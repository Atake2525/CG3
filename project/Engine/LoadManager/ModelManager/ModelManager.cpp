#include "ModelManager.h"
#include "Model.h"
#include "ModelBase.h"
#include "DirectXBase.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance() {
	if (instance == nullptr) {
		instance = new ModelManager;
	}
	return instance;
}

void ModelManager::Finalize() {
	delete instance;
	instance = nullptr;
}

void ModelManager::Initialize(DirectXBase* directxBase) { 
	ModelBase::GetInstance()->Initialize(directxBase); 
}

void ModelManager::LoadModel(const std::string& directoryPath, const std::string& filePath, const bool& enableLighting) {
	// 読み込み済モデルを検索
	if (models.contains(filePath)) {
		// 読み込み済なら早期return
		return;
	}

	// モデルの生成と読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(directoryPath, filePath, enableLighting);

	// モデルをmapコンテナに格納する
	models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath) {
	// 読み込み済モデルを検索
	if (models.contains(filePath)) {
	// 読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}

	// ファイル名一致無し
	return nullptr;
}