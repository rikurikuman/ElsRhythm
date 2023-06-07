#include "SRBuffer.h"
#include "RDirectX.h"
#include <Util.h>

std::recursive_mutex SRBufferAllocator::mutex;
bool SRBufferAllocator::optAutoDeflag = false;
bool SRBufferAllocator::optAutoReCreateBuffer = true;

SRBufferAllocator* SRBufferAllocator::GetInstance()
{
	static SRBufferAllocator instance;
	return &instance;
}

UINT64 Align(UINT64 location, UINT64 align) {
	if ((align == 0) || (align & (align - 1))) {
		return 0;
	}
	return (location + (align - 1)) & ~(align - 1);
}

SRBufferPtr SRBufferAllocator::Alloc(UINT64 needSize, UINT align)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->mutex);

	MemoryRegion* reg = nullptr;

	reg = _Alloc(needSize, align, optAutoDeflag);
	if (reg == nullptr) return SRBufferPtr();

	instance->regionPtrs.emplace_back();
	instance->regionPtrs.back().region = reg;
	instance->regionPtrs.back().memItr = --instance->regionPtrs.end();
	return SRBufferPtr(&instance->regionPtrs.back());
}

void SRBufferAllocator::Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->mutex);

	if (ptr.ptr == nullptr || ptr.ptr->region == nullptr) {
#ifdef _DEBUG
		OutputDebugStringA(Util::StringFormat("RKEngine WARNING: SRBufferAllocator::Free() : Attempted to free an invalid pointer(%p).\n", ptr.ptr).c_str());
#endif
		return;
	}

	_Free(ptr);
	//もっと安全に消せるように何とかする
	ptr.ptr->region = nullptr;
	instance->regionPtrs.erase(ptr.ptr->memItr);
	ptr.ptr = nullptr;
}

MemoryRegion* SRBufferAllocator::_Alloc(UINT64 needSize, UINT align, bool deflag)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->mutex);

	//確保した領域の先頭アドレスを指すポインタ
	byte* newLoc = nullptr;

	//空き領域から欲しいサイズが収まる領域を探す
	for (MemoryRegion& reg : instance->freeRegions) {

		//要求境界にアライメントする
		byte* alignedLoc = reinterpret_cast<byte*>(Align(reinterpret_cast<UINT64>(reg.pBegin), align));

		//アライメントに失敗するようならnullのまま返す
		if (alignedLoc == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alignment.\n");
#endif
			return nullptr;
		}

		//アライメントされた後のアドレスから要求サイズが確保できるか確認
		UINT64 remainSize = UINT64(reg.pEnd - alignedLoc + 1);
		if (reg.pEnd < alignedLoc || remainSize < needSize) {
			continue; //できないなら別の空き領域でもう一度
		}

		//できるなら採用
		newLoc = alignedLoc;
		break;
	}

	//確保できなかったら
	if (newLoc == nullptr) {
		if (deflag) {
			//デフラグして再確保を試みる
			instance->DeFlag();
			return _Alloc(needSize, align, false);
		}
		if (optAutoReCreateBuffer) {
			//より大きいサイズでバッファを作り直して再確保を試みる
			instance->ResizeBuffer();
			return _Alloc(needSize, align, false);
		}
		//可能な限り頑張ったけど無理だったのでごめんねする
#ifdef _DEBUG
		OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alloc. Out of memory.\n");
#endif
		return nullptr;
	}

	//空き領域情報を編集する
	//確保領域の先頭アドレスが含まれている空き領域を探す
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
		MemoryRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//あれば、空き領域の先頭から確保領域の先頭までを新たな空き領域として追加する
		if (UINT64(newLoc - reg.pBegin) != 0) {
			itr = instance->freeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//あれば、確保領域の末尾から空き領域の末尾までを新たな空き領域として追加する
		if (UINT64(reg.pEnd - (newLoc + needSize - 1)) != 0) {
			itr = instance->freeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//空き領域削除
		instance->freeRegions.erase(itr);
		break;
	}

	instance->usingRegions.push_back(MemoryRegion(newLoc, newLoc + needSize - 1, align));
	instance->usingRegions.back().memItr = --instance->usingRegions.end();
	instance->usingBufferSizeCounter += needSize;
	return &instance->usingRegions.back();
}

void SRBufferAllocator::_Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->mutex);

	//使用中領域から指定アドレスを先頭とする領域を探す
	//bool regionFound = false;
	MemoryRegion usingRegion;
	//for (auto itr = instance->usingRegions.begin(); itr != instance->usingRegions.end(); itr++) {
	//	if (itr->pBegin == ptr) {
	//		//見つけたら覚えて、消す
	//		regionFound = true;
	//		usingRegion = *itr;
	//		instance->usingRegions.erase(itr);

	//		//消す時に分かりやすく0xddにする
	//		for (byte* ptr = usingRegion.pBegin; ptr <= usingRegion.pEnd; ptr++) {
	//			*ptr = 0xdd;
	//		}
	//		break;
	//	}
	//}
	usingRegion = *ptr.ptr->region->memItr;
	instance->usingRegions.erase(ptr.ptr->region->memItr);
	//消す時に分かりやすく0xddにする
	for (byte* ptr = usingRegion.pBegin; ptr <= usingRegion.pEnd; ptr++) {
		*ptr = 0xdd;
	}

	//見つからなかったら不正なポインタを解放しようとしてるので怒る
//	if (!regionFound) {
//		//けど今は警告出力してreturnでいいや
//#ifdef _DEBUG
//		OutputDebugStringA(Util::StringFormat("RKEngine WARNING: SRBufferAllocator::Free() : Attempted to free an invalid pointer(%p).\n", ptr).c_str());
//#endif
//		return;
//	}

	//解放する領域を空き領域に追加する
	auto prev = instance->freeRegions.end();
	auto next = instance->freeRegions.end();
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
		//解放する領域の先頭と連続する一番近い既存の空き領域を探す
		if (itr->pEnd <= usingRegion.pBegin) {
			prev = itr;
		}

		//解放する領域の末尾と連続する一番近い既存の空き領域を探す
		if (usingRegion.pEnd <= itr->pBegin) {
			next = itr;
			break;
		}
	}

	//前方領域がある時
	bool prevMerged = false;
	if (prev != instance->freeRegions.end()) {
		//完全に連続するなら
		if (prev->pEnd == usingRegion.pBegin - 1) {
			//融合する
			prev->pEnd = usingRegion.pEnd;
			prev->size = prev->pEnd - prev->pBegin + 1;
			prevMerged = true;
		}
	}

	//後方領域がある時
	bool nextMerged = false;
	if (next != instance->freeRegions.end()) {
		//完全に連続するなら
		if (next->pBegin == usingRegion.pEnd + 1) {
			//前方領域と融合済みかで分岐して
			if (prevMerged) {
				//融合済みなら更に融合
				prev->pEnd = next->pEnd;
				prev->size = prev->pEnd - prev->pBegin + 1;
				instance->freeRegions.erase(next);
				prevMerged = true;
			}
			else {
				//融合してないなら普通に融合
				next->pBegin = usingRegion.pBegin;
				next->size = next->pEnd - next->pBegin + 1;
				prevMerged = true;
			}
		}
	}

	//融合してないなら孤立した領域なので追加する
	if (!prevMerged && !nextMerged) {
		if (next != instance->freeRegions.end()) {
			//後ろに領域があるならその前に挿入
			instance->freeRegions.insert(next, MemoryRegion(usingRegion.pBegin, usingRegion.pEnd));
		}
		else {
			//ないならプッシュ
			instance->freeRegions.emplace_back(usingRegion.pBegin, usingRegion.pEnd);
		}
	}

	instance->usingBufferSizeCounter -= usingRegion.size;
}

void SRBufferAllocator::DeFlag()
{
	std::lock_guard<std::recursive_mutex> lock(mutex);
	freeRegions.clear();
	freeRegions.emplace_back(pBufferBegin, pBufferEnd);

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : regionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	usingRegions.clear();
	usingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : regionPtrs) {
		MemoryRegion& old = map[&ptr];
		auto newReg = _Alloc(old.size, old.align, false);
		if (newReg == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::DeFlag() : Failed Realloc.\n");
#endif
			continue;
		}
		memmove_s(newReg->pBegin, newReg->size, old.pBegin, old.size);
		ptr.region = newReg;
	}

	for (MemoryRegion& reg : freeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}
}

void SRBufferAllocator::ResizeBuffer() {
	std::lock_guard<std::recursive_mutex> lock(mutex);

	// 再度確保
	Microsoft::WRL::ComPtr<ID3D12Resource> newBuff;
	size_t rebufferSize = GetBufferSize() * 2; //とりあえず2倍のサイズに
	HRESULT result;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = rebufferSize;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newBuff)
	);
	assert(SUCCEEDED(result));

	result = newBuff->Map(0, nullptr, (void**)&pBufferBegin); //マッピング
	assert(SUCCEEDED(result));

	pBufferEnd = pBufferBegin + rebufferSize - 1;

	freeRegions.clear();
	freeRegions.emplace_back(pBufferBegin, pBufferEnd);
	for (byte* ptr = pBufferBegin; ptr <= pBufferEnd; ptr++) {
		*ptr = 0xdd;
	}

	// 確保終わり、既存データ移行

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : regionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	usingRegions.clear();
	usingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : regionPtrs) {
		MemoryRegion& old = map[&ptr];
		auto newReg = _Alloc(old.size, old.align, false);
		if (newReg == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::ResizeBuffer() : Failed Realloc.\n");
#endif
			continue;
		}
		memmove_s(newReg->pBegin, newReg->size, old.pBegin, old.size);
		ptr.region = newReg;
	}

	for (MemoryRegion& reg : freeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}

	buffer = newBuff;
}

SRBufferAllocator::SRBufferAllocator() {
	size_t bufferSize = defSize;//(defSize + 0xff) & ~0xff; //256バイトアラインメント

	// 確保
	HRESULT result;

	// ヒープ設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = bufferSize; 
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 生成
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer)
	);
	assert(SUCCEEDED(result));

	result = buffer->Map(0, nullptr, (void**)&pBufferBegin); //マッピング
	assert(SUCCEEDED(result));

	pBufferEnd = pBufferBegin + bufferSize - 1;

	freeRegions.emplace_back(pBufferBegin, pBufferEnd);
	for (byte* ptr = pBufferBegin; ptr <= pBufferEnd; ptr++) {
		*ptr = 0xdd;
	}
}

byte* SRBufferPtr::Get() {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::mutex);
	if (ptr != nullptr && ptr->region != nullptr) return ptr->region->pBegin;
	return nullptr;
}

D3D12_GPU_VIRTUAL_ADDRESS SRBufferPtr::GetGPUVirtualAddress() const
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::mutex);
	D3D12_GPU_VIRTUAL_ADDRESS address = SRBufferAllocator::GetGPUVirtualAddress();
	address += static_cast<UINT>(ptr->region->pBegin - SRBufferAllocator::GetBufferAddress());
	return address;
}

SRBufferPtr::operator bool() const {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::mutex);
	return ptr != nullptr && ptr->region != nullptr;
}