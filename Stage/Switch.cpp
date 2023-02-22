#include "Switch.h"

void Switch::Initialize(Model* model) {
	// ƒ‚ƒfƒ‹“Ç‚Ýž‚Ý
	model_ = model;

	isFlag_ = false;
}

void Switch::Update() {
	isFlag_ = false;
}

void Switch::Draw(ViewProjection viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}

void Switch::OnCollisionSwitch() {
	isFlag_ = true;
}