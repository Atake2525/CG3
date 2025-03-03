#include "DebugMode.h"

using namespace Microsoft::WRL;

#include "externels/imgui/imgui.h"
 #include "externels/imgui/imgui_impl_dx12.h"
 #include "externels/imgui/imgui_impl_win32.h"

void DebugMode::Initialize() {

	FrameWork::Initialize();

#pragma region 基盤システムの初期化

	winApp = new WinApp();
	winApp->Initialize();

	directxBase = new DirectXBase();
	directxBase->Initialize(winApp);

	camera = new Camera();
	camera->SetRotate({0.36f, 0.0f, 0.0f});
	camera->SetTranslate({0.0f, 6.0f, -19.0f});

	SpriteBase::GetInstance()->Initialize(directxBase);

	Object3dBase::GetInstance()->Initialize(directxBase);
	Object3dBase::GetInstance()->SetDefaultCamera(camera);

	ModelBase::GetInstance()->Initialize(directxBase);

	TextureManager::GetInstance()->Initialize(directxBase);

	ModelManager::GetInstance()->Initialize(directxBase);

#pragma endregion 基盤システムの初期化

	TextureManager::GetInstance()->LoadTexture("Resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("Resources/monsterBall.png");
	sprite = new Sprite();
	sprite->Initialize("Resources/uvChecker.png");
	sprite->SetScale(Vector2{200.0f, 200.0f});

	//uint32_t textureIndexSphere = TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/monsterBall.png");
	//uint32_t textureIndexUv = TextureManager::GetInstance()->GetTextureIndexByFilePath("Resources/uvChecker.png");

	ModelManager::GetInstance()->LoadModel("Resources", "terrain.obj");
	// ModelManager::GetInstance()->LoadModel("Resources", "axis.obj");
	// ModelManager::GetInstance()->LoadModel("Resources", "bunny.obj");
	// ModelManager::GetInstance()->LoadModel("Resources", "teapot.obj");

	Audio::GetInstance()->Initialize();

	soundData1 = Audio::GetInstance()->SoundLoadWave("Resources/Alarm01.wav");

	object3d = new Object3d();
	object3d->Initialize();

	object3d->SetModel("terrain.obj");

	input = new Input();
	input->Initialize(winApp);

	directionalLightResource = directxBase->CreateBufferResource(sizeof(DirectionalLight));

	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	directionalLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLightData->direction = {0.0f, -1.0f, 0.0f};
	directionalLightData->intensity = 1.0f;

	pointLightResource = directxBase->CreateBufferResource(sizeof(PointLight));

	pointLightResource->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData));

	pointLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	pointLightData->position = {0.0f, 2.0f, 0.0f};
	pointLightData->intensity = 0.0f;
	pointLightData->radius = 5.0f;
	pointLightData->dacay = 5.0f;

	spotLightResource = directxBase->CreateBufferResource(sizeof(SpotLight));

	spotLightResource->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData));

	spotLightData->color = {1.0f, 1.0f, 1.0f, 1.0f};
	spotLightData->position = {2.0f, 1.25f, 0.0f};
	spotLightData->distance = 7.0f;
	spotLightData->direction = Normalize({-1.0f, -1.0f, 0.0f});
	spotLightData->intensity = 0.0f;
	spotLightData->dacay = 2.0f;
	spotLightData->cosAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	spotLightData->cosFalloffStart = std::cos(std::numbers::pi_v<float> / 2.6f);

	// Transform変数を作る
	Transform transform{
	    {1.0f, 1.0f,   1.0f},
        {0.0f, -1.58f, 0.0f},
        {0.0f, 0.0f,   0.0f}
    };

	Transform cameraTransform{
	    {1.0f,  1.0f, 1.0f  },
        {0.36f, 0.0f, 0.0f  },
        {0.0f,  6.0f, -19.0f}
    };

	// Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.1f, 100.0f);

	// カメラ用のリソースを作成 Phong Reflection Model
	cameraResource = directxBase->CreateBufferResource(sizeof(CameraForGPU));
	// 書き込むためのアドレスを取得
	cameraResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraData));

	cameraData->worldPosition = cameraTransform.translate;

	position = sprite->GetPosition();
	rotation = sprite->GetRotation();
	scale = sprite->GetScale();
	color = sprite->GetColor();
	anchorPoint = sprite->GetAnchorPoint();
	isFlipX = sprite->GetIsFlipX();
	isFlipY = sprite->GetIsFlipY();
	textureLeftTop = sprite->GetTextureLeftTop();
	textureSize = sprite->GetTextureSize();

	modelTransform = object3d->GetTransform();
	modelTransform.rotate = object3d->GetRotateInDegree();
	modelColor = object3d->GetColor();
	modelEnableLighting = object3d->GetEnableLighting();
}

void DebugMode::Update() {

	FrameWork::Update();

	if (winApp->ProcessMessage()) {
		Finished = true;
	}
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Imguiの設定(Color&STR)
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.7f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.0f, 0.0f, 0.7f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(300, 400));
	ImGui::Begin("colorConfig");

	if (ImGui::TreeNode("ColorCode")) {
		ImGui::DragFloat3("RGB", &modelColor.x, 0.01f);
		ImGui::ColorEdit3("RGB", &modelColor.x);
		// ImGui::ColorEdit3("RGBSprite", &color.x);
		ImGui::SliderFloat("R", &modelColor.x, 0.0f, 1.0f);
		ImGui::SliderFloat("G", &modelColor.y, 0.0f, 1.0f);
		ImGui::SliderFloat("B", &modelColor.z, 0.0f, 1.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("colorPreset")) {
		if (ImGui::Button("RED")) {
			modelColor.x = 1.0f;
			modelColor.y = 0.0f;
			modelColor.z = 0.0f;
		}
		if (ImGui::Button("GREEN")) {
			modelColor.x = 0.0f;
			modelColor.y = 1.0f;
			modelColor.z = 0.0f;
		}
		if (ImGui::Button("BLUE")) {
			modelColor.x = 0.0f;
			modelColor.y = 0.0f;
			modelColor.z = 1.0f;
		}
		if (ImGui::Button("YELLOW")) {
			modelColor.x = 1.0f;
			modelColor.y = 1.0f;
			modelColor.z = 0.0f;
		}
		if (ImGui::Button("WHITE")) {
			modelColor.x = 1.0f;
			modelColor.y = 1.0f;
			modelColor.z = 1.0f;
		}
		if (ImGui::Button("BLACK")) {
			modelColor.x = 0.0f;
			modelColor.y = 0.0f;
			modelColor.z = 0.0f;
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("STR")) {
		ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.01f);
		ImGui::DragFloat3("Translate", &transform.translate.x, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ModelSTR")) {
		ImGui::DragFloat3("Scale", &modelTransform.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate", &modelTransform.rotate.x, 1.0f);
		ImGui::DragFloat3("Translate", &modelTransform.translate.x, 0.01f);
		ImGui::Checkbox("EnableLighting", &modelEnableLighting);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("SpriteSTR")) {
		ImGui::DragFloat2("Scale", &scale.x, 1.0f);
		ImGui::DragFloat("Rotate", &rotation, 0.01f);
		ImGui::DragFloat2("Translate", &position.x, 1.0f);
		ImGui::DragFloat2("AnchorPoint", &anchorPoint.x, 0.1f);
		ImGui::Checkbox("FlipX", &isFlipX);
		ImGui::Checkbox("FlipY", &isFlipY);
		ImGui::DragFloat2("TextureLeftTop", &textureLeftTop.x, 0.1f);
		ImGui::DragFloat2("TextureSize", &textureSize.x, 0.1f);
		// ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		// ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
		// ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Lighting")) {
		// ImGui::ColorEdit3("SpecularColor", &materialData->specularColor.x);
		if (ImGui::TreeNode("DirectionalLight")) {
			ImGui::ColorEdit4("Color", &directionalLightData->color.x);
			ImGui::SliderFloat3("Direction", &directionalLightData->direction.x, -5.0f, 5.0f);
			ImGui::DragFloat("Insensity", &directionalLightData->intensity, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("PointLight")) {
			ImGui::ColorEdit4("Color", &pointLightData->color.x);
			ImGui::DragFloat3("Positoin", &pointLightData->position.x, 0.1f);
			ImGui::SliderFloat("radius", &pointLightData->radius, 0.0f, 10.0f);
			ImGui::SliderFloat("dacay", &pointLightData->dacay, 0.0f, 10.0f);
			ImGui::DragFloat("Insensity", &pointLightData->intensity, 1.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("SpotLight")) {
			ImGui::ColorEdit4("Color", &spotLightData->color.x);
			ImGui::DragFloat3("Positoin", &spotLightData->position.x, 0.1f);
			ImGui::DragFloat3("Direction", &spotLightData->direction.x, 0.1f);
			ImGui::DragFloat("cosAngle", &spotLightData->cosAngle, 0.01f);
			ImGui::DragFloat("cosFalloffStart", &spotLightData->cosFalloffStart, 0.01f);
			ImGui::SliderFloat("distance", &spotLightData->distance, 0.0f, 10.0f);
			ImGui::SliderFloat("dacay", &spotLightData->dacay, 0.0f, 10.0f);
			ImGui::DragFloat("Insensity", &spotLightData->intensity, 1.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(1080, 0));
	ImGui::SetNextWindowSize(ImVec2(200, 300));
	ImGui::Begin("Camera");

	ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.01f);
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.01f);

	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

#ifdef _DEBUG
	const float speed = 0.7f;
	Vector3 velocity(0.0f, 0.0f, speed);
	velocity = TransformNormal(velocity, camera->GetWorldMatrix());
	if (input->PushKey(DIK_W)) {
		cameraTransform.translate += velocity;
	}
	if (input->PushKey(DIK_S)) {
		cameraTransform.translate -= velocity;
	}
	velocity = {speed, 0.0f, 0.0f};
	velocity = TransformNormal(velocity, camera->GetWorldMatrix());
	if (input->PushKey(DIK_A)) {
		cameraTransform.translate -= velocity;
	}
	if (input->PushKey(DIK_D)) {
		cameraTransform.translate += velocity;
	}
	if (input->PushKey(DIK_SPACE)) {
		cameraTransform.translate.y += 1.0f;
	}
	if (input->PushKey(DIK_LSHIFT)) {
		cameraTransform.translate.y -= 1.0f;
	}
	if (input->PushKey(DIK_LEFT)) {
		cameraTransform.rotate.y -= 0.03f;
	}
	if (input->PushKey(DIK_RIGHT)) {
		cameraTransform.rotate.y += 0.03f;
	}
	if (input->PushKey(DIK_UP)) {
		cameraTransform.rotate.x -= 0.03f;
	}
	if (input->PushKey(DIK_DOWN)) {
		cameraTransform.rotate.x += 0.03f;
	}
	if (input->PushKey(DIK_Q)) {
		cameraTransform.rotate.z -= 0.01f;
	}
	if (input->PushKey(DIK_E)) {
		cameraTransform.rotate.z += 0.01f;
	}

	cameraTransform.rotate.x = std::clamp(cameraTransform.rotate.x, SwapRadian(-90.0f), SwapRadian(90.0f));

	if (input->TriggerKey(DIK_0)) {
		// 音声再生
		Audio::GetInstance()->SoundPlayWave(soundData1);
	}
	if (input->TriggerKey(DIK_ESCAPE)) {
		Finished = true;
	}
#endif // _DEBUG

}

void DebugMode::Draw() {
	// ImGuiの内部コマンドを生成する
	ImGui::Render();

	camera->SetRotate(cameraTransform.rotate);
	camera->SetTranslate(cameraTransform.translate);
	camera->Update();

	sprite->SetStatus(position, rotation, scale, color);
	sprite->SetAnchorPoint(anchorPoint);
	sprite->SetIsFlip(isFlipX, isFlipY);
	sprite->SetTextureLeftTop(textureLeftTop);
	sprite->SetTextureSize(textureSize);
	sprite->Update();

	object3d->SetDirectionalLight(directionalLightData);
	object3d->SetPointLight(pointLightData);
	object3d->SetSpotLight(spotLightData);

	object3d->SetTransform(modelTransform);
	object3d->SetRotateInDegree(modelTransform.rotate);
	object3d->SetColor(modelColor);
	object3d->SetEnableLighting(modelEnableLighting);
	object3d->Update();

	directxBase->PreDraw();

	SpriteBase::GetInstance()->ShaderDraw();

	sprite->Draw();

	Object3dBase::GetInstance()->ShaderDraw();

	object3d->Draw(directionalLightResource, pointLightResource);

	// 実際のcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), directxBase->GetCommandList().Get());

	input->Update();

	directxBase->PostDraw();
}

void DebugMode::Finalize() {
	winApp->Finalize();
	delete winApp;

	directxBase->Finalize();
	delete directxBase;

	delete camera;

	SpriteBase::GetInstance()->Finalize();

	Object3dBase::GetInstance()->Finalize();

	ModelBase::GetInstance()->Finalize();

	TextureManager::GetInstance()->Finalize();

	ModelManager::GetInstance()->Finalize();

	delete sprite;

	Audio::GetInstance()->SoundUnload(&soundData1);
	Audio::GetInstance()->Finalize();

	delete object3d;

	delete input;

	FrameWork::Finalize();
}