#include "SimpleDrawer.h"
#include "RDirectX.h"
#include "RWindow.h"
#include "TextDrawer.h"
#include <chrono>
#include <Renderer.h>

using namespace std;

void SimpleDrawer::ClearData()
{
	SimpleDrawer* instance = GetInstance();
	instance->infoList.clear();
	for (auto& itr : instance->recycleBuffs) {
		SRBufferAllocator::Free(*itr.second);
	}
	instance->recycleBuffs.clear();

	instance->boxInfoMap.clear();
	instance->boxBuffersMap.clear();
	instance->lineInfoMap.clear();
	instance->lineBuffersMap.clear();
	instance->circleInfoMap.clear();
	instance->circleVertIndexMap.clear();
	instance->circleBuffersMap.clear();
}


void SimpleDrawer::DrawLine(int x1, int y1, int x2, int y2, float layer, Color color, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	//既にそのレイヤーで描画が要求されてるか探す
	auto itr = instance->lineInfoMap.find(layer);
	if (itr != instance->lineInfoMap.end()) {
		//同じレイヤーに他の描画がいたら同じ描画コマンドで描画するように要素を追加する
		std::vector<DrawLineInfo>& list = itr->second;
		list.push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), thickness, color });
		return;
	}
	//されてないみたいなので新規描画コマンドを追加する
	instance->lineInfoMap[layer].push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), thickness, color });
}

void SimpleDrawer::DrawBox(int x1, int y1, int x2, int y2, float layer, Color color, bool fillFlag, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	if (fillFlag) {
		//4頂点ポリゴン描画

		//既にそのレイヤーで描画が要求されてるか探す
		auto itr = instance->boxInfoMap.find(layer);
		if (itr != instance->boxInfoMap.end()) {
			//同じレイヤーに他の描画がいたら同じ描画コマンドで描画するように要素を追加する
			std::vector<DrawBoxInfo>& list = itr->second;
			list.push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), color });
			return;
		}
		//されてないみたいなので新規描画コマンドを追加する
		instance->boxInfoMap[layer].push_back({ Vector2(static_cast<float>(x1), static_cast<float>(y1)), Vector2(static_cast<float>(x2), static_cast<float>(y2)), color });
	}
	else {
		//DrawLineによる描画
		int adjX1 = static_cast<int>(x1 + thickness / 2.0f);
		int adjX2 = static_cast<int>(x2 - thickness / 2.0f);
		int adjY1 = static_cast<int>(y1 + thickness / 2.0f);
		int adjY2 = static_cast<int>(y2 - thickness / 2.0f);

		SimpleDrawer::DrawLine(x1, adjY1, x2, adjY1, layer, color, thickness); //左上から右上
		SimpleDrawer::DrawLine(x1, adjY2, x2, adjY2, layer, color, thickness); //左下から右下
		SimpleDrawer::DrawLine(adjX1, y1, adjX1, y2, layer, color, thickness); //左上から左下
		SimpleDrawer::DrawLine(adjX2, y1, adjX2, y2, layer, color, thickness); //右上から右下
	}
}

void SimpleDrawer::DrawCircle(int x, int y, int r, float layer, Color color, bool fillFlag, float thickness)
{
	SimpleDrawer* instance = GetInstance();

	//今回のカスタムデータだ
	DrawCustomData cData = { layer, fillFlag, static_cast<float>(r) };
	
	if (fillFlag) {
		//頂点とそのインデックスを必要なら計算する
		if (instance->circleBuffersMap.find(cData) == instance->circleBuffersMap.end()) {
			instance->CalcCircleVertAndIndex(cData);
		}
	}
	else {
		if (instance->circleBuffersMap.find(cData) == instance->circleBuffersMap.end()) {
			DrawCustomData cDataMin = cData; //内側の小さめな円
			DrawCustomData cDataMax = cData; //外側の大きめな円
			cDataMin.radius -= thickness / 2.0f;
			cDataMax.radius += thickness / 2.0f;

			//頂点とそのインデックスを必要なら計算する
			if (instance->circleBuffersMap.find(cDataMin) == instance->circleBuffersMap.end()) {
				instance->CalcCircleVertAndIndex(cDataMin);
			}
			if (instance->circleBuffersMap.find(cDataMax) == instance->circleBuffersMap.end()) {
				instance->CalcCircleVertAndIndex(cDataMax);
			}

			//二つの円の頂点を融合して最強にする
			std::vector<VertexP> vertices;
			std::vector<UINT> indices;

			std::vector<VertexP>& vertMin = instance->circleVertIndexMap[cDataMin].vert;
			std::vector<VertexP>& vertMax = instance->circleVertIndexMap[cDataMax].vert;
			for (int i = 0; i < vertMin.size() - 1; i++) {
				vertices.push_back(vertMin[i]);
			}
			for (int i = 0; i < vertMax.size() - 1; i++) {
				vertices.push_back(vertMax[i]);
			}

			int minI = 0;
			int maxI = static_cast<int>(vertMin.size());
			while (minI < vertMin.size() - 1 || maxI < vertMax.size()) {
				if (minI < vertMin.size() - 1) {
					indices.push_back(minI);
					indices.push_back(minI + 1);
					indices.push_back(maxI);
					minI++;
				}
				
				if (maxI < vertices.size() - 1) {
					indices.push_back(maxI);
					indices.push_back(maxI + 1);
					indices.push_back(minI);
					maxI++;
				}
			}
			indices.push_back(0);
			indices.push_back(maxI);
			indices.push_back(minI + 1);

			instance->circleVertIndexMap[cData].vert = vertices;
			instance->circleVertIndexMap[cData].index = indices;
			instance->circleBuffersMap[cData].vert.Init(vertices);
			instance->circleBuffersMap[cData].index.Init(indices);
		}
	}

	instance->circleInfoMap[cData].push_back({ Vector2(x, y), color });
}

void SimpleDrawer::DrawString(float x, float y, float layer, std::string text, Color color, std::string fontTypeFace, UINT fontSize, Vector2 anchor)
{
	SimpleDrawer* instance = GetInstance();
	shared_ptr<DrawStringInfo> info = make_shared<DrawStringInfo>();
	TextureHandle tex = TextDrawer::CreateStringTexture(text, fontTypeFace, fontSize);
	info->sprite = Sprite(tex, anchor);
	info->sprite.transform.position = { x, y, layer };
	info->sprite.transform.UpdateMatrix();
	info->sprite.material.color = color;
	info->sprite.TransferBuffer();

	info->sprite.Draw();

	TextureManager::UnRegisterAtEndFrame(tex);
	instance->infoList.push_back(info);
}

void SimpleDrawer::DrawAll() {
	SimpleDrawer* instance = GetInstance();

	for (auto itr = instance->boxInfoMap.begin(); itr != instance->boxInfoMap.end(); itr++) {
		float layer = itr->first;

		struct vert {
			Vector2 pos;
			Color color;
		};
		std::vector<vert> vertices;
		std::vector<UINT> indices;
		int index = 0;
		for (DrawBoxInfo& info : itr->second) {
			Matrix4 mat = Matrix4::OrthoGraphicProjection(
				0, (float)RWindow::GetWidth(),
				0, (float)RWindow::GetHeight(),
				0, 1);
			Vector3 leftTop = Vector2(info.start.x, info.start.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 rightTop = Vector2(info.end.x, info.start.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 leftBottom = Vector2(info.start.x, info.end.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			Vector3 rightBottom = Vector2(info.end.x, info.end.y) - Vector2(RWindow::GetWidth() / 2.0f, RWindow::GetHeight() / 2.0f);
			leftTop *= mat;
			rightTop *= mat;
			leftBottom *= mat;
			rightBottom *= mat;
			vertices.push_back({ leftTop, info.color });
			vertices.push_back({ rightTop, info.color });
			vertices.push_back({ leftBottom, info.color });
			vertices.push_back({ rightBottom, info.color });
			indices.emplace_back(index);
			indices.emplace_back(index + 3);
			indices.emplace_back(index + 1);
			indices.emplace_back(index);
			indices.emplace_back(index + 2);
			indices.emplace_back(index + 3);
			index += 4;
		}

		instance->boxBuffersMap[layer].vert.Update(vertices);
		instance->boxBuffersMap[layer].index.Init(indices);

		RenderOrder order;
		order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		order.vertBuff = instance->boxBuffersMap[layer].vert;
		order.indexBuff = instance->boxBuffersMap[layer].index;
		order.indexCount = instance->boxBuffersMap[layer].index.GetIndexCount();
		order.rootSignature = instance->boxRS.ptr.Get();
		order.pipelineState = instance->boxPSO.ptr.Get();
		
		string stage;
		if (layer >= 0) {
			//前面
			order.anchorPoint.z = layer;
			stage = "Sprite";
		}
		else {
			//背面
			order.anchorPoint.z = -layer;
			stage = "BackSprite";
		}
		Renderer::DrawCall(stage, order);
	}

	if (!instance->lineInfoMap.empty()) {
		Matrix4 mat = Matrix4::OrthoGraphicProjection(
			0, (float)RWindow::GetWidth(),
			0, (float)RWindow::GetHeight(),
			0, 1);

		instance->lineVPBuff->matrix = mat;

		for (auto itr = instance->lineInfoMap.begin(); itr != instance->lineInfoMap.end(); itr++) {
			float layer = itr->first;

			struct vert {
				Vector2 pos;
				Color color;
				float thickness;
			};
			std::vector<vert> vertices;
			for (DrawLineInfo& info : itr->second) {
				Vector3 start = Vector2(info.start.x, info.start.y);
				Vector3 goal = Vector2(info.end.x, info.end.y);
				vertices.push_back({ start, info.color, info.thickness });
				vertices.push_back({ goal, info.color, info.thickness });
			}

			instance->lineBuffersMap[layer].vert.Update(vertices);

			RenderOrder order;
			order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
			order.vertBuff = instance->lineBuffersMap[layer].vert;
			order.indexCount = static_cast<UINT>(vertices.size());
			order.rootData = {
				{RootDataType::SRBUFFER_CBV, instance->lineVPBuff.buff }
			};
			order.rootSignature = instance->lineRS.ptr.Get();
			order.pipelineState = instance->linePSO.ptr.Get();

			string stage;
			if (layer >= 0) {
				//前面
				order.anchorPoint.z = layer;
				stage = "Sprite";
			}
			else {
				//背面
				order.anchorPoint.z = -layer;
				stage = "BackSprite";
			}
			Renderer::DrawCall(stage, order);
		}
	}
	
	if (!instance->circleInfoMap.empty()) {
		Matrix4 mat = Matrix4::OrthoGraphicProjection(
			0, (float)RWindow::GetWidth(),
			0, (float)RWindow::GetHeight(),
			0, 1);

		instance->circleVPBuff->matrix = mat;

		for (auto itr = instance->circleInfoMap.begin(); itr != instance->circleInfoMap.end(); itr++) {
			float layer = itr->first.layer;

			instance->circleBuffersMap[itr->first].instance.Update(itr->second);

			RenderOrder order;
			order.primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			order.vertBuff = instance->circleBuffersMap[itr->first].vert;
			order.indexBuff = instance->circleBuffersMap[itr->first].index;
			order.indexCount = static_cast<UINT>(instance->circleBuffersMap[itr->first].index.GetIndexCount());
			order.instanceVertBuff = instance->circleBuffersMap[itr->first].instance;
			order.instanceCount = static_cast<UINT>(itr->second.size());
			order.rootData = {
				{RootDataType::SRBUFFER_CBV, instance->circleVPBuff.buff }
			};
			order.rootSignature = instance->circleRS.ptr.Get();
			order.pipelineState = instance->circlePSO.ptr.Get();

			string stage;
			if (layer >= 0) {
				//前面
				order.anchorPoint.z = layer;
				stage = "Sprite";
			}
			else {
				//背面
				order.anchorPoint.z = -layer;
				stage = "BackSprite";
			}
			Renderer::DrawCall(stage, order);
		}
	}
}

void SimpleDrawer::Init()
{
	rootSignature = RDirectX::GetDefRootSignature();

	// ルートパラメータの設定
	RootParamaters rootParams(2);
	//定数バッファ0番(Color)
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	rootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	//定数バッファ2番(ViewProjection)
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 1; //定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0; //デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える

	rootSignature.desc.RootParamaters = rootParams;
	rootSignature.Create();

	pipelineState = RDirectX::GetDefPipeline();

	pipelineState.desc.VS = Shader("./Shader/SimpleVS.hlsl", "main", "vs_5_0");
	pipelineState.desc.PS = Shader("./Shader/SimplePS.hlsl", "main", "ps_5_0");

	// ラスタライザの設定
	pipelineState.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	pipelineState.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	pipelineState.desc.RasterizerState.DepthClipEnable = false;

	pipelineState.desc.DepthStencilState.DepthEnable = false;

	pipelineState.desc.pRootSignature = rootSignature.ptr.Get();

	pipelineState.Create();

	//DrawBox用
	boxRS.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	boxRS.Create();

	boxPSO.desc.VS = Shader("./Shader/SimpleMeshVS.hlsl", "main", "vs_5_0");
	boxPSO.desc.PS = Shader("./Shader/SimpleMeshPS.hlsl", "main", "ps_5_0");
	boxPSO.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	boxPSO.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	boxPSO.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	boxPSO.desc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = boxPSO.desc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	boxPSO.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	boxPSO.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	boxPSO.desc.DepthStencilState.DepthEnable = false;
	boxPSO.desc.NumRenderTargets = 1;
	boxPSO.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	boxPSO.desc.SampleDesc.Count = 1;
	boxPSO.desc.pRootSignature = boxRS.ptr.Get();
	boxPSO.Create();

	//DrawLine用
	lineRS.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	RootParamaters lineRootParams(1);
	lineRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	lineRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	lineRootParams[0].Descriptor.RegisterSpace = 0;
	lineRootParams[0].Descriptor.ShaderRegister = 0;
	lineRS.desc.RootParamaters = lineRootParams;
	lineRS.Create();

	linePSO.desc.VS = Shader("./Shader/SimpleLineVS.hlsl", "main", "vs_5_0");
	linePSO.desc.GS = Shader("./Shader/SimpleLineGS.hlsl", "main", "gs_5_0");
	linePSO.desc.PS = Shader("./Shader/SimpleLinePS.hlsl", "main", "ps_5_0");
	linePSO.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	linePSO.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	linePSO.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	linePSO.desc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = linePSO.desc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	linePSO.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"THICKNESS", 0, DXGI_FORMAT_R32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	linePSO.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	linePSO.desc.DepthStencilState.DepthEnable = false;
	linePSO.desc.NumRenderTargets = 1;
	linePSO.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	linePSO.desc.SampleDesc.Count = 1;
	linePSO.desc.pRootSignature = lineRS.ptr.Get();
	linePSO.Create();

	//DrawCircle用
	circleRS.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	RootParamaters circleRootParams(1);
	circleRootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //定数バッファビュー
	circleRootParams[0].Descriptor.ShaderRegister = 0; //定数バッファ番号
	circleRootParams[0].Descriptor.RegisterSpace = 0; //デフォルト値
	circleRootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //全シェーダから見える
	circleRS.desc.RootParamaters = circleRootParams;
	circleRS.desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	circleRS.Create();

	circlePSO.desc.VS = Shader("Shader/SimpleInstanceVS.hlsl", "main", "vs_5_0");
	circlePSO.desc.PS = Shader("Shader/SimpleInstancePS.hlsl", "main", "ps_5_0");
	circlePSO.desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	circlePSO.desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	circlePSO.desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	circlePSO.desc.RasterizerState.DepthClipEnable = true;

	{
		auto& blendstate = circlePSO.desc.BlendState.RenderTarget[0];
		blendstate.BlendEnable = true;
		blendstate.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendstate.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlendAlpha = D3D12_BLEND_ONE;
		blendstate.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendstate.BlendOp = D3D12_BLEND_OP_ADD;
		blendstate.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blendstate.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	}

	circlePSO.desc.InputLayout = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 1
		},
	};

	circlePSO.desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	circlePSO.desc.DepthStencilState.DepthEnable = false;
	circlePSO.desc.NumRenderTargets = 1;
	circlePSO.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	circlePSO.desc.SampleDesc.Count = 1;
	circlePSO.desc.pRootSignature = lineRS.ptr.Get();
	circlePSO.Create();

	//DrawString用
	rootSignatureForString = SpriteManager::GetInstance()->GetRootSignature();

	StaticSamplerDesc samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; //ボーダーの時は黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //リニア補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; //ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; //ミップマップ最小値
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //ピクセルシェーダーからだけ見える
	rootSignatureForString.desc.StaticSamplers = StaticSamplerDescs{ samplerDesc };
	rootSignatureForString.Create();

	pipelineStateForString = SpriteManager::GetInstance()->GetGraphicsPipeline();
	pipelineStateForString.desc.pRootSignature = rootSignatureForString.ptr.Get();
	pipelineStateForString.Create();
}

void SimpleDrawer::CalcCircleVertAndIndex(DrawCustomData cData) {
	SimpleDrawer* instance = GetInstance();

	std::vector<VertexP> vertices;
	std::vector<UINT> indices;

	float ang = 0;
	for (int i = 0; i < 360; i++) {
		float plus = 360.0f / 360;
		vertices.push_back(VertexP({ cData.radius * cosf(Util::AngleToRadian(ang)), cData.radius * sinf(Util::AngleToRadian(ang)), 0 }));
		vertices.push_back(VertexP({ cData.radius * cosf(Util::AngleToRadian(ang + plus)), cData.radius * sinf(Util::AngleToRadian(ang + plus)), 0 }));
		ang += plus;
	}
	vertices.push_back(VertexP({ 0, 0, 0 }));

	for (int i = 0; i < vertices.size() - 2; i++) {
		indices.push_back(i);
		indices.push_back(i + 1);
		indices.push_back(static_cast<UINT>(vertices.size() - 1));
	}

	//sin, cosを使わない手法
	//(intの計算ループの方が早いだろうから速度意識するならこっちか？)
	/*int cx = 0;
	int cy = r;
	int d = 1 - r;
	int dH = 3;
	int dD = 5 - 2 * r;

	for (cx = 0; cx <= cy; cx++) {
		if (d < 0) {
			d += dH;
			dH += 2;
			dD += 2;
		}
		else {
			d += dD;
			dH += 2;
			dD += 4;
			--cy;
		}

		vertices.push_back(VertexP({ static_cast<float>(cy), static_cast<float>(cx), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(cx), static_cast<float>(cy), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(-cx), static_cast<float>(cy), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(-cy), static_cast<float>(cx), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(-cy), static_cast<float>(-cx), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(-cx), static_cast<float>(-cy), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(cx), static_cast<float>(-cy), 0 }));
		vertices.push_back(VertexP({ static_cast<float>(cy), static_cast<float>(-cx), 0 }));
	}
	vertices.push_back(VertexP({ 0, 0, 0 }));

	for (int i = 0; i < 8; i++) {
		UINT index = i;
		UINT count = 0;

		while (count < vertices.size() / 8 - 1) {
			indices.push_back(index);
			indices.push_back(index + 8);
			indices.push_back(static_cast<UINT>(vertices.size() - 1));
			index += 8;
			count++;
		}
	}
	indices.push_back(0);*/

	instance->circleVertIndexMap[cData].vert = vertices;
	instance->circleVertIndexMap[cData].index = indices;
	instance->circleBuffersMap[cData].vert.Init(vertices);
	instance->circleBuffersMap[cData].index.Init(indices);
}