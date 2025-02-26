#include "Sprite.h"
#include "SpriteBase.h"
#include "DirectXBase.h"
#include "TextureManager.h"

void Sprite::SetTransform(Transform& transform){ 
	position.x = transform.translate.x;
	position.y = transform.translate.y;
	rotation = transform.rotate.z;
	scale.x = transform.rotate.x;
	scale.y = transform.rotate.y;
}

//void Sprite::SetMaterial(Material* material){ 
//	materialData = material; 
//}

void Sprite::SetStatus(const Vector2& position, const float& rotation, const Vector2& scale, const Vector4& color){ 
	this->position = position; 
	this->rotation = rotation;
	this->scale = scale;
	materialData->color = color;
}

void Sprite::SetTransform(const Vector2& position, const float& rotation, const Vector2& scale) {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}


void Sprite::Initialize(std::string textureFilePath) { 

	// VertexResourceの作成
	CreateVertexResource();
	// IndexResourceの作成
	CreateIndexResource();
	// MaterialResourceの作成
	CreateMaterialResource();
	// TransformationMatrixResourceの作成
	CreateTransformationMatrixResource();
	// VertexBufferViewの作成
	CreateVertexBufferView();
	// IndexBufferViewの作成
	CreateIndexBufferView();

	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	// MaterialBufferViewの作成
	SetMaterial();
	// TransformationMatrixBufferViewの作成
	SetTransformatinMatrix();

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
	AdjustTextureSize();
}

void Sprite::Update() {

	// アンカーポイントの設定
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	// 左右上下フリップの設定

	// 左右反転
	if (isFlipX) {
		left = -left;
		right = -right;
	}
	// 上下反転
	if (isFlipY) {
		top = -top;
		bottom = -bottom;
	}

	// テクスチャ範囲指定の設定
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;

	// sprite(頂点データ)の設定
	vertexData[0].position = {left, top, 0.0f, 1.0f}; // 左上
	vertexData[0].texcoord = {tex_left, tex_top};
	vertexData[1].position = {right, top, 0.0f, 1.0f}; // 右上
	vertexData[1].texcoord = {tex_right, tex_top};
	vertexData[2].position = {right, bottom, 0.0f, 1.0f}; // 右下
	vertexData[2].texcoord = {tex_right, tex_bottom};
	vertexData[3].position = {left, bottom, 0.0f, 1.0f}; // 左下
	vertexData[3].texcoord = {tex_left, tex_bottom};

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 3;
	indexData[3] = 3;
	indexData[4] = 1;
	indexData[5] = 2;

	Transform transform{
	      {1.0f, 1.0f, 1.0f},
          {0.0f, 0.0f, 0.0f},
          {0.0f, 0.0f, 0.0f}
    };

	Transform uvTransform{
	    {1.0f, 1.0f, 1.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	};

	transform.translate = {position.x, position.y, 0.0f};
	transform.rotate = {0.0f, 0.0f, rotation};
	transform.scale = {scale.x, scale.y, 0.1f};



	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(uvTransform.scale);
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvTransform.rotate.z));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(uvTransform.translate));
	materialData->uvTransform = uvTransformMatrix;

	// ゲームの処理
	//  Sprite用のWorldViewProjectionMatrixを作る
	//  SpriteのTransform処理
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
}

void Sprite::ChangeTexture(std::string textureFilePath) { 
	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
}

void Sprite::Draw() {
	// Spriteの描画。変更が必要なものだけ変更する
	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定

	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexbufferView); // IBVを設定

	// マテリアルCBufferの場所を設定
	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	// TransformationMatrixCBbufferの場所を設定
	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());
	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex));
	// 描画
	SpriteBase::GetInstance()->GetDxBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

void Sprite::CreateIndexResource() { 
	indexResource = SpriteBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(uint32_t) * 6);
}

void Sprite::CreateVertexResource() { 
	vertexResource = SpriteBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(VertexData) * 6); 
}

void Sprite::CreateMaterialResource() { 
	materialResource = SpriteBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(Material)); 
}

void Sprite::CreateTransformationMatrixResource() { 
	transformationMatrixResource = SpriteBase::GetInstance()->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix)); 
}

void Sprite::CreateIndexBufferView() {
	// リソースの先頭のアドレスから使う
	indexbufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexbufferView.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexbufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Sprite::CreateVertexBufferView() {
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);
}

void Sprite::SetMaterial() {
	// マテリアルデータの初期値を書き込む
	materialData->color = Vector4{1.0f, 1.0f, 1.0f, 1.0f};
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();
}

void Sprite::SetTransformatinMatrix() {
	// 単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
}

void Sprite::SetIsFlip(const bool& FlipX, const bool& FlipY) {
	isFlipX = FlipX;
	isFlipY = FlipY;
}

void Sprite::AdjustTextureSize() {
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex);

	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	// 画像サイズをテクスチャサイズに合わせる
	scale = textureSize;
}