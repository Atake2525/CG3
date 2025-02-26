#include "ModelBase.h"
#include "DirectXBase.h"

ModelBase* ModelBase::instance = nullptr;

ModelBase* ModelBase::GetInstance() {
	if (instance == nullptr) {
		instance = new ModelBase;
	}
	return instance;
}

void ModelBase::Finalize() {
	delete instance;
	instance = nullptr;
}
 
void ModelBase::Initialize(DirectXBase* directxBase) { 
	directxBase_ = directxBase;
}