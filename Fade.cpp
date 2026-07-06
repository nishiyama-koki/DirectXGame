#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize() {
	sprite_ = Sprite::Create(0, {0.0f, 0.0f});
	sprite_->SetSize({1280.0f, 720.0f});
	sprite_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		break;
	case Status::FadeIn:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
			status_ = Status::None;
		}
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->SetColor(Vector4(0.0f, 0.0f, 0.0f, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() { status_ = Status::None; }

bool Fade::IsFinished() const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	return true;
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}
	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}