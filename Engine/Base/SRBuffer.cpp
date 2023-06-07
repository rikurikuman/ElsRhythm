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
	//�����ƈ��S�ɏ�����悤�ɉ��Ƃ�����
	ptr.ptr->region = nullptr;
	instance->regionPtrs.erase(ptr.ptr->memItr);
	ptr.ptr = nullptr;
}

MemoryRegion* SRBufferAllocator::_Alloc(UINT64 needSize, UINT align, bool deflag)
{
	SRBufferAllocator* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(instance->mutex);

	//�m�ۂ����̈�̐擪�A�h���X���w���|�C���^
	byte* newLoc = nullptr;

	//�󂫗̈悩��~�����T�C�Y�����܂�̈��T��
	for (MemoryRegion& reg : instance->freeRegions) {

		//�v�����E�ɃA���C�����g����
		byte* alignedLoc = reinterpret_cast<byte*>(Align(reinterpret_cast<UINT64>(reg.pBegin), align));

		//�A���C�����g�Ɏ��s����悤�Ȃ�null�̂܂ܕԂ�
		if (alignedLoc == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA("RKEngine ERROR: SRBufferAllocator::_Alloc() : Failed Alignment.\n");
#endif
			return nullptr;
		}

		//�A���C�����g���ꂽ��̃A�h���X����v���T�C�Y���m�ۂł��邩�m�F
		UINT64 remainSize = UINT64(reg.pEnd - alignedLoc + 1);
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
		if (optAutoReCreateBuffer) {
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
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
		MemoryRegion& reg = *itr;
		if (newLoc < reg.pBegin || reg.pEnd < newLoc) {
			continue;
		}

		//����΁A�󂫗̈�̐擪����m�ۗ̈�̐擪�܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (UINT64(newLoc - reg.pBegin) != 0) {
			itr = instance->freeRegions.emplace(itr, reg.pBegin, newLoc - 1);
			itr++;
		}

		//����΁A�m�ۗ̈�̖�������󂫗̈�̖����܂ł�V���ȋ󂫗̈�Ƃ��Ēǉ�����
		if (UINT64(reg.pEnd - (newLoc + needSize - 1)) != 0) {
			itr = instance->freeRegions.emplace(itr++, newLoc + needSize, reg.pEnd);
			itr++;
		}

		//�󂫗̈�폜
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
	usingRegion = *ptr.ptr->region->memItr;
	instance->usingRegions.erase(ptr.ptr->region->memItr);
	//�������ɕ�����₷��0xdd�ɂ���
	for (byte* ptr = usingRegion.pBegin; ptr <= usingRegion.pEnd; ptr++) {
		*ptr = 0xdd;
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
	auto prev = instance->freeRegions.end();
	auto next = instance->freeRegions.end();
	for (auto itr = instance->freeRegions.begin(); itr != instance->freeRegions.end(); itr++) {
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
	if (prev != instance->freeRegions.end()) {
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
	if (next != instance->freeRegions.end()) {
		//���S�ɘA������Ȃ�
		if (next->pBegin == usingRegion.pEnd + 1) {
			//�O���̈�ƗZ���ς݂��ŕ��򂵂�
			if (prevMerged) {
				//�Z���ς݂Ȃ�X�ɗZ��
				prev->pEnd = next->pEnd;
				prev->size = prev->pEnd - prev->pBegin + 1;
				instance->freeRegions.erase(next);
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
		if (next != instance->freeRegions.end()) {
			//���ɗ̈悪����Ȃ炻�̑O�ɑ}��
			instance->freeRegions.insert(next, MemoryRegion(usingRegion.pBegin, usingRegion.pEnd));
		}
		else {
			//�Ȃ��Ȃ�v�b�V��
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

	result = newBuff->Map(0, nullptr, (void**)&pBufferBegin); //�}�b�s���O
	assert(SUCCEEDED(result));

	pBufferEnd = pBufferBegin + rebufferSize - 1;

	freeRegions.clear();
	freeRegions.emplace_back(pBufferBegin, pBufferEnd);
	for (byte* ptr = pBufferBegin; ptr <= pBufferEnd; ptr++) {
		*ptr = 0xdd;
	}

	// �m�ۏI���A�����f�[�^�ڍs

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
	size_t bufferSize = defSize;//(defSize + 0xff) & ~0xff; //256�o�C�g�A���C�������g

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
		IID_PPV_ARGS(&buffer)
	);
	assert(SUCCEEDED(result));

	result = buffer->Map(0, nullptr, (void**)&pBufferBegin); //�}�b�s���O
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