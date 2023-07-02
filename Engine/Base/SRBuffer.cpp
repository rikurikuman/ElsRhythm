#include "SRBuffer.h"
#include "RDirectX.h"
#include <Util.h>

std::recursive_mutex SRBufferAllocator::sMutex;
bool SRBufferAllocator::mOptAutoDeflag = false;
bool SRBufferAllocator::mOptAutoReCreateBuffer = true;

SRBufferAllocator* SRBufferAllocator::GetInstance()
{
	static SRBufferAllocator instance;
	return &instance;
}

size_t Align(size_t location, size_t align) {
	if ((align == 0) || (align & (align - 1))) {
		return 0;
	}
	return (location + (align - 1)) & ~(align - 1);
}

SRBufferPtr SRBufferAllocator::Alloc(size_t needSize, uint32_t align)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	MemoryRegion* reg = nullptr;

	reg = _Alloc(needSize, align, mOptAutoDeflag);
	if (reg == nullptr) return SRBufferPtr();

	instance->mRegionPtrs.emplace_back();
	instance->mRegionPtrs.back().region = reg;
	instance->mRegionPtrs.back().memItr = --instance->mRegionPtrs.end();
	return SRBufferPtr(&instance->mRegionPtrs.back());
}

void SRBufferAllocator::Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	if (ptr.mPtr == nullptr || ptr.mPtr->region == nullptr) {
#ifdef _DEBUG
		OutputDebugStringA(Util::StringFormat("RKEngine WARNING: SRBufferAllocator::Free() : Attempted to free an invalid pointer(%p).\n", ptr.mPtr).c_str());
#endif
		return;
	}

	_Free(ptr);
	//�����ƈ��S�ɏ�����悤�ɉ��Ƃ�����
	ptr.mPtr->region = nullptr;
	instance->mRegionPtrs.erase(ptr.mPtr->memItr);
	ptr.mPtr = nullptr;
}

MemoryRegion* SRBufferAllocator::_Alloc(size_t needSize, uint32_t align, bool deflag)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	//�m�ۂ����̈�̐擪�A�h���X���w���|�C���^
	byte* newLoc = nullptr;

	//�󂫗̈悩��~�����T�C�Y�����܂�̈��T��
	for (MemoryRegion& reg : instance->mFreeRegions) {

		//�v�����E�ɃA���C�����g����
		byte* alignedLoc = reinterpret_cast<byte*>(Align(reinterpret_cast<size_t>(reg.pBegin), align));

		//�A���C�����g�Ɏ��s����悤�Ȃ�null�̂܂ܕԂ�
		if (alignedLoc == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alignment.\n");
#endif
			return nullptr;
		}

		//�A���C�����g���ꂽ��̃A�h���X����v���T�C�Y���m�ۂł��邩�m�F
		size_t remainSize = size_t(reg.pEnd - alignedLoc + 1);
		if (reg.pEnd < alignedLoc || remainSize < needSize) {
			continue; //�ł��Ȃ��Ȃ�ʂ̋󂫗̈�ł�����x
		}

		//�ł���Ȃ�̗p
		newLoc = alignedLoc;
		break;
	}

	//�m�ۂł��Ȃ�������
	if (newLoc == nullptr) {
		if (deflag) {
			//�f�t���O���čĊm�ۂ����݂�
			instance->DeFlag();
			return _Alloc(needSize, align, false);
		}
		if (mOptAutoReCreateBuffer) {
			//���傫���T�C�Y�Ńo�b�t�@����蒼���čĊm�ۂ����݂�
			instance->ResizeBuffer();
			return _Alloc(needSize, align, false);
		}
		//�\�Ȍ���撣�������ǖ����������̂ł��߂�˂���
#ifdef _DEBUG
		OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alloc. Out of memory.\n");
#endif
		return nullptr;
	}

	//�󂫗̈����ҏW����
	//�m�ۗ̈�̐擪�A�h���X���܂܂�Ă���󂫗̈��T��
	for (auto itr = instance->mFreeRegions.begin(); itr != instance->mFreeRegions.end(); itr++) {
		MemoryRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//����΁A�󂫗̈�̐擪����m�ۗ̈�̐擪�܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (size_t(newLoc - reg.pBegin) != 0) {
			itr = instance->mFreeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//����΁A�m�ۗ̈�̖�������󂫗̈�̖����܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (size_t(reg.pEnd - (newLoc + needSize - 1)) != 0) {
			itr = instance->mFreeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//�󂫗̈�폜
		instance->mFreeRegions.erase(itr);
		break;
	}

	instance->mUsingRegions.push_back(MemoryRegion(newLoc, newLoc + needSize - 1, align));
	instance->mUsingRegions.back().memItr = --instance->mUsingRegions.end();
	instance->mUsingBufferSizeCounter += needSize;
	return &instance->mUsingRegions.back();
}

void SRBufferAllocator::_Free(SRBufferPtr& ptr)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->sMutex);

	//�g�p���̈悩��w��A�h���X��擪�Ƃ���̈��T��
	//bool regionFound = false;
	MemoryRegion usingRegion;
	//for (auto itr = instance->usingRegions.begin(); itr != instance->usingRegions.end(); itr++) {
	//	if (itr->pBegin == ptr) {
	//		//��������o���āA����
	//		regionFound = true;
	//		usingRegion = *itr;
	//		instance->usingRegions.erase(itr);

	//		//�������ɕ�����₷��0xdd�ɂ���
	//		for (byte* ptr = usingRegion.pBegin; ptr <= usingRegion.pEnd; ptr++) {
	//			*ptr = 0xdd;
	//		}
	//		break;
	//	}
	//}
	usingRegion = *ptr.mPtr->region->memItr;
	instance->mUsingRegions.erase(ptr.mPtr->region->memItr);
	//�������ɕ�����₷��0xdd�ɂ���
	for (byte* p = usingRegion.pBegin; p <= usingRegion.pEnd; p++) {
		*p = 0xdd;
	}

	//������Ȃ�������s���ȃ|�C���^��������悤�Ƃ��Ă�̂œ{��
//	if (!regionFound) {
//		//���Ǎ��͌x���o�͂���return�ł�����
//#ifdef _DEBUG
//		OutputDebugStringA(Util::StringFormat("RKEngine WARNING: SRBufferAllocator::Free() : Attempted to free an invalid pointer(%p).\n", ptr).c_str());
//#endif
//		return;
//	}

	//�������̈���󂫗̈�ɒǉ�����
	auto prev = instance->mFreeRegions.end();
	auto next = instance->mFreeRegions.end();
	for (auto itr = instance->mFreeRegions.begin(); itr != instance->mFreeRegions.end(); itr++) {
		//�������̈�̐擪�ƘA�������ԋ߂������̋󂫗̈��T��
		if (itr->pEnd <= usingRegion.pBegin) {
			prev = itr;
		}

		//�������̈�̖����ƘA�������ԋ߂������̋󂫗̈��T��
		if (usingRegion.pEnd <= itr->pBegin) {
			next = itr;
			break;
		}
	}

	//�O���̈悪���鎞
	bool prevMerged = false;
	if (prev != instance->mFreeRegions.end()) {
		//���S�ɘA������Ȃ�
		if (prev->pEnd == usingRegion.pBegin - 1) {
			//�Z������
			prev->pEnd = usingRegion.pEnd;
			prev->size = prev->pEnd - prev->pBegin + 1;
			prevMerged = true;
		}
	}

	//����̈悪���鎞
	bool nextMerged = false;
	if (next != instance->mFreeRegions.end()) {
		//���S�ɘA������Ȃ�
		if (next->pBegin == usingRegion.pEnd + 1) {
			//�O���̈�ƗZ���ς݂��ŕ��򂵂�
			if (prevMerged) {
				//�Z���ς݂Ȃ�X�ɗZ��
				prev->pEnd = next->pEnd;
				prev->size = prev->pEnd - prev->pBegin + 1;
				instance->mFreeRegions.erase(next);
				prevMerged = true;
			}
			else {
				//�Z�����ĂȂ��Ȃ畁�ʂɗZ��
				next->pBegin = usingRegion.pBegin;
				next->size = next->pEnd - next->pBegin + 1;
				prevMerged = true;
			}
		}
	}

	//�Z�����ĂȂ��Ȃ�Ǘ������̈�Ȃ̂Œǉ�����
	if (!prevMerged && !nextMerged) {
		if (next != instance->mFreeRegions.end()) {
			//���ɗ̈悪����Ȃ炻�̑O�ɑ}��
			instance->mFreeRegions.insert(next, MemoryRegion(usingRegion.pBegin, usingRegion.pEnd));
		}
		else {
			//�Ȃ��Ȃ�v�b�V��
			instance->mFreeRegions.emplace_back(usingRegion.pBegin, usingRegion.pEnd);
		}
	}

	instance->mUsingBufferSizeCounter -= usingRegion.size;
}

void SRBufferAllocator::DeFlag()
{
	std::lock_guard<std::recursive_mutex> lock(sMutex);
	mFreeRegions.clear();
	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	mUsingRegions.clear();
	mUsingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
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

	for (MemoryRegion& reg : mFreeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}
}

void SRBufferAllocator::ResizeBuffer() {
	std::lock_guard<std::recursive_mutex> lock(sMutex);

	// �ēx�m��
	Microsoft::WRL::ComPtr<ID3D12Resource> newBuff;
	size_t rebufferSize = GetBufferSize() * 2; //�Ƃ肠����2�{�̃T�C�Y��
	HRESULT result;

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = rebufferSize;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&newBuff)
	);
	assert(SUCCEEDED(result));

	result = newBuff->Map(0, nullptr, (void**)&mPtrBufferBegin); //�}�b�s���O
	assert(SUCCEEDED(result));

	mPtrBufferEnd = mPtrBufferBegin + rebufferSize - 1;

	mFreeRegions.clear();
	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);
	for (byte* ptr = mPtrBufferBegin; ptr <= mPtrBufferEnd; ptr++) {
		*ptr = 0xdd;
	}

	// �m�ۏI���A�����f�[�^�ڍs

	std::map<MemoryRegionPtr*, MemoryRegion> map;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
		map[&ptr] = MemoryRegion(*ptr.region);
	}
	mUsingRegions.clear();
	mUsingBufferSizeCounter = 0;
	for (MemoryRegionPtr& ptr : mRegionPtrs) {
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

	for (MemoryRegion& reg : mFreeRegions) {
		for (byte* ptr = reg.pBegin; ptr <= reg.pEnd; ptr++) {
			*ptr = 0xdd;
		}
	}

	mBuffer = newBuff;
}

SRBufferAllocator::SRBufferAllocator() {
	size_t bufferSize = DEFAULT_SIZE;//(defSize + 0xff) & ~0xff; //256�o�C�g�A���C�������g

	// �m��
	HRESULT result;

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = bufferSize; 
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// ����
	result = RDirectX::GetDevice()->CreateCommittedResource(
		&cbHeapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&mBuffer)
	);
	assert(SUCCEEDED(result));

	result = mBuffer->Map(0, nullptr, (void**)&mPtrBufferBegin); //�}�b�s���O
	assert(SUCCEEDED(result));

	mPtrBufferEnd = mPtrBufferBegin + bufferSize - 1;

	mFreeRegions.emplace_back(mPtrBufferBegin, mPtrBufferEnd);
	for (byte* ptr = mPtrBufferBegin; ptr <= mPtrBufferEnd; ptr++) {
		*ptr = 0xdd;
	}
}

byte* SRBufferPtr::Get() {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	if (mPtr != nullptr && mPtr->region != nullptr) return mPtr->region->pBegin;
	return nullptr;
}

D3D12_GPU_VIRTUAL_ADDRESS SRBufferPtr::GetGPUVirtualAddress() const
{
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	D3D12_GPU_VIRTUAL_ADDRESS address = SRBufferAllocator::GetGPUVirtualAddress();
	address += static_cast<uint32_t>(mPtr->region->pBegin - SRBufferAllocator::GetBufferAddress());
	return address;
}

SRBufferPtr::operator bool() const {
	std::lock_guard<std::recursive_mutex> lock(SRBufferAllocator::sMutex);
	return mPtr != nullptr && mPtr->region != nullptr;
}