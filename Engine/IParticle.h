#pragma once
#include <functional>

class IParticle
{
public:
	bool mRemoveFlag = false;

	virtual ~IParticle() {};

	//パーティクル一つ一つの更新処理
	virtual void Update() = 0;

	//パーティクル一つ一つの描画処理
	virtual void Draw() = 0;

	//パーティクル一種類毎のバッチ描画処理への関数ポインタ取得関数
	//Draw関数内でDrawCallする場合はoverride不要
	virtual std::function<void()> GetBatchDrawFunc() { return nullptr; };

	//パーティクル一種類毎のバッチ描画用データのクリア処理への関数ポインタ取得関数
	//バッチ描画処理をしていないならoverride不要
	virtual std::function<void()> GetBatchClearFunc() { return nullptr; };
};

