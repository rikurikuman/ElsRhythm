#include "CollidersScene.h"
#include <TimeManager.h>
#include <RImGui.h>
#include <Quaternion.h>
#include <Renderer.h>

CollidersScene::CollidersScene()
{
	sphere = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere", true));
	sphere2 = ModelObj(Model::Load("./Resources/Model/", "Sphere.obj", "Sphere2", true));
	ray = ModelObj(Model::Load("./Resources/Model/", "Cube.obj", "Cube", false));

	sphere.mTransform.position = { 0, 5, 0 };
	sphere.mTransform.UpdateMatrix();
	sphere2.mTransform.position = { 2, 3, 0 };
	sphere2.mTransform.UpdateMatrix();

	colSphere = Colliders::Create<SphereCollider>(Vector3(0, 5, 0), 1.0f);
	colSphere2 = Colliders::Create<SphereCollider>(Vector3(2, 3, 0), 1.0f);
	colPolygon = Colliders::Create<PolygonCollider>(posA, posC, posB);
	colRay = Colliders::Create<RayCollider>(Vector3(0, 7, 0), Vector3(0, -1, 0));
	colRay->mFuncOnCollision = [&](CollisionInfo info) {
		//念のためチェック
		if (info.hasDistance && info.hasInter) {
			if (info.distance < memDis) {
				memDis = info.distance;
			}
		}
	};

	polygonPipeline = RDirectX::GetDefPipeline();
	polygonPipeline.mDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	polygonPipeline.Create();

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Init(verts, 3);

	camera.mViewProjection.mEye = { 0, 3, -10 };
	camera.mViewProjection.mTarget = { 0, 3, 0 };
	camera.mViewProjection.UpdateMatrix();
}

void CollidersScene::Init()
{
	Camera::sNowCamera = &camera;
	LightGroup::sNowLight = &light;
	Colliders::AllActivate();
}

void CollidersScene::Update()
{
	//テストGUI
	{
		ImGui::SetNextWindowPos({ ImGui::GetMainViewport()->WorkPos.x + 800, ImGui::GetMainViewport()->WorkPos.y + 10 }, ImGuiCond_Once);
		ImGui::SetNextWindowSize({ 400, 500 });

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoResize;
		ImGui::Begin("Control", NULL, window_flags);

		if (ImGui::Button("Reset")) {
			sphere.mTransform.position = { 0, 5, 0 };
			sphere.mTransform.scale = { 1, 1, 1 };
			sphere2.mTransform.position = { 2, 3, 0 };
			sphere2.mTransform.scale = { 1, 1, 1 };
			colRay->mRay.start = { 0, 7, 0 };
			radiusA = 1;
			radiusB = 1;
			pierce = false;

			posA = { 0, 0, 1 };
			posB = { -1, 0, -1 };
			posC = { 1, 0, -1 };
		}

		ImGui::Text("SphereA");
		ImGui::Checkbox("AutoMove##SphereA", &autoMoveA);
		ImGui::DragFloat3("Position##SphereA", &sphere.mTransform.position.x, 0.01f);
		ImGui::DragFloat("Scale##SphereA", &radiusA, 0.01f);

		ImGui::Text("SphereB");
		ImGui::Checkbox("AutoMove##SphereB", &autoMoveB);
		ImGui::DragFloat3("Position##SphereB", &sphere2.mTransform.position.x, 0.01f);
		ImGui::DragFloat("Scale##SphereB", &radiusB, 0.01f);

		ImGui::Text("Ray");
		ImGui::Checkbox("AutoMove##Ray", &autoMoveC);
		ImGui::Checkbox("Pierce", &pierce);
		ImGui::DragFloat3("Position##Ray", &colRay->mRay.start.x, 0.01f);

		ImGui::Text("Polygon");
		ImGui::DragFloat3("Position1", &posA.x, 0.01f);
		ImGui::DragFloat3("Position2", &posB.x, 0.01f);
		ImGui::DragFloat3("Position3", &posC.x, 0.01f);

		ImGui::End();
	}

	if (autoMoveA) {
		sphere.mTransform.position.x += 5.0f * TimeManager::deltaTime * moveDirA;
		if (sphere.mTransform.position.x > 5
			|| sphere.mTransform.position.x < -5) {
			sphere.mTransform.position.x = Util::Clamp(sphere.mTransform.position.x, -5.0f, 5.0f);
			moveDirA *= -1;
		}
	}

	if (autoMoveB) {
		sphere2.mTransform.position.x += 5.0f * TimeManager::deltaTime * moveDirB;
		if (sphere2.mTransform.position.x > 5
			|| sphere2.mTransform.position.x < -5) {
			sphere2.mTransform.position.x = Util::Clamp(sphere2.mTransform.position.x, -5.0f, 5.0f);
			moveDirB *= -1;
		}
	}

	if (autoMoveC) {
		colRay->mRay.start.x += 5.0f * TimeManager::deltaTime * moveDirC;
		if (colRay->mRay.start.x > 5
			|| colRay->mRay.start.x < -5) {
			colRay->mRay.start.x = Util::Clamp(colRay->mRay.start.x, -5.0f, 5.0f);
			moveDirC *= -1;
		}
	}

	colSphere->mSphere.pos = sphere.mTransform.position;
	colSphere->mSphere.r = radiusA;
	colSphere2->mSphere.pos = sphere2.mTransform.position;
	colSphere2->mSphere.r = radiusB;

	sphere.mTransform.scale = { radiusA, radiusA, radiusA };
	sphere.mTransform.UpdateMatrix();
	sphere2.mTransform.scale = { radiusB, radiusB, radiusB };
	sphere2.mTransform.UpdateMatrix();

	colPolygon->mPolygon.p0 = posA;
	colPolygon->mPolygon.p1 = posC;
	colPolygon->mPolygon.p2 = posB;

	colRay->mOptPierce = pierce;

	memDis = FLT_MAX;
	Colliders::Update();

	if (colSphere->HasCollision()) {
		sphere.mTuneMaterial.mColor = { 1, 0, 0, 1 };
	}
	else {
		sphere.mTuneMaterial.mColor = { 1, 1, 1, 1 };
	}

	if (colSphere2->HasCollision()) {
		sphere2.mTuneMaterial.mColor = { 1, 0, 0, 1 };
	}
	else {
		sphere2.mTuneMaterial.mColor = { 1, 1, 1, 1 };
	}

	if (pierce) {
		if (colRay->HasCollision()) {
			ray.mTuneMaterial.mColor = { 0, 0, 1, 1 };
		}
		else {
			ray.mTuneMaterial.mColor = { 1, 1, 1, 1 };
		}
		ray.mTransform.position = colRay->mRay.start;
		ray.mTransform.position += colRay->mRay.dir * 50;
		ray.mTransform.scale = { 0.1f, 100, 0.1f };
	}
	else {
		if (colRay->HasCollision()) {
			ray.mTuneMaterial.mColor = { 0, 0, 1, 1 };
			ray.mTransform.position = colRay->mRay.start;
			ray.mTransform.position += colRay->mRay.dir * memDis / 2.0f;
			ray.mTransform.scale = { 0.1f, memDis, 0.1f };
		}
		else {
			ray.mTuneMaterial.mColor = { 1, 1, 1, 1 };
			ray.mTransform.position = colRay->mRay.start;
			ray.mTransform.position += colRay->mRay.dir * 50;
			ray.mTransform.scale = { 0.1f, 100, 0.1f };
		}
	}
	ray.mTransform.UpdateMatrix();

	if (colPolygon->HasCollision()) {
		materialBuff.mConstMap->color = { 1, 0, 0, 1 };
	}
	else {
		materialBuff.mConstMap->color = { 1, 1, 1, 1 };
	}

	light.Update();
	sphere.TransferBuffer(Camera::sNowCamera->mViewProjection);
	sphere2.TransferBuffer(Camera::sNowCamera->mViewProjection);
	ray.TransferBuffer(Camera::sNowCamera->mViewProjection);

	VertexPNU verts[3] = {
		{posA, {0, 0, 0}, {0, 0}},
		{posC, {0, 0, 0}, {0, 0}},
		{posB, {0, 0, 0}, {0, 0}}
	};
	vertBuff.Update(verts, 3);
	transformBuff.mConstMap->matrix = Matrix4();
	materialBuff.mConstMap->ambient = { 1, 1, 1 };
	Camera::sNowCamera->mViewProjection.Transfer(viewProjectionBuff.mConstMap);
}

void CollidersScene::Draw()
{
	sphere.Draw();
	sphere2.Draw();
	ray.Draw();

	RenderOrder polygon;
	polygon.pipelineState = polygonPipeline.mPtr.Get();
	polygon.rootData = {
		{TextureManager::Get("").mGpuHandle},
		{RootDataType::CBV, materialBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, transformBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, viewProjectionBuff.mConstBuff->GetGPUVirtualAddress()},
		{RootDataType::CBV, LightGroup::sNowLight->mBuffer.mConstBuff->GetGPUVirtualAddress()},
	};
	polygon.vertView = &vertBuff.mView;
	polygon.indexCount = 3;

	Renderer::DrawCall("Opaque", polygon);
}
