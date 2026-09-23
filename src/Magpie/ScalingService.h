#pragma once
#include "Event.h"
#include "ScalingOptions.h"
#include "ScalingRuntime.h"
#include "Singleton.h"
#include <winrt/Magpie.h>

namespace Magpie {

struct Profile;

class ScalingService : public Singleton<ScalingService> {
	friend Singleton<ScalingService>;

public:
	~ScalingService();

	void Initialize();

	void Uninitialize() noexcept;

	void StartTimer(bool windowedMode);

	void StopTimer();

	bool IsTimerOn() const noexcept {
		return _curCountdownSeconds > 0;
	}

	bool IsTimerOn(bool windowedMode) const noexcept {
		return IsTimerOn() && windowedMode == _isCurCountdownWindowedMode;
	}

	double TimerProgress() const noexcept {
		return SecondsLeft() / _curCountdownSeconds;
	}

	double SecondsLeft() const noexcept;

	bool IsScaling() const noexcept;

	// 当前正在缩放（或等待缩放）的 Profile，未在缩放时为 nullptr
	const Profile* CurScalingProfile() const noexcept {
		return _curProfile;
	}

	// 用于 Auto-adjust 按钮，阻塞直至获得结果
	OutputFitInfo GetOutputFitInfo() noexcept {
		return _scalingRuntime ? _scalingRuntime->GetOutputFitInfo() : OutputFitInfo{};
	}

	// 强制重新检查前台窗口
	void CheckForeground();

	Event<bool, bool> IsTimerOnChanged;
	Event<double> TimerTick;
	Event<bool> IsScalingChanged;

private:
	ScalingService() = default;

	void _ShortcutService_ShortcutPressed(winrt::Magpie::ShortcutAction action);

	void _CountDownTimer_Tick(winrt::DispatcherQueueTimer const&, winrt::IInspectable const&);

	void _CheckForegroundTimer_Tick(winrt::DispatcherQueueTimer const&, winrt::IInspectable const&);

	void _ScalingRuntime_StateChanged(ScalingState value);

	void _ScaleForegroundWindow(bool windowedMode);

	void _StartScale(HWND hWnd, const Profile& profile, bool windowedMode, bool force);

	ScalingError _StartScaleImpl(HWND hWnd, const Profile& profile, bool windowedMode, bool force);

	std::optional<ScalingRuntime> _scalingRuntime;

	winrt::DispatcherQueueTimer _countDownTimer{ nullptr };
	winrt::DispatcherQueueTimer _checkForegroundTimer{ nullptr };

	Event<winrt::Magpie::ShortcutAction>::EventRevoker _shortcutActivatedRevoker;

	std::chrono::steady_clock::time_point _timerStartTimePoint;

	uint32_t _curCountdownSeconds = 0;
	bool _isCurCountdownWindowedMode = false;

	HWND _hwndCurSrc = NULL;
	const Profile* _curProfile = nullptr;
	// 1. 避免重复检查同一个窗口
	// 2. 用户使用热键退出全屏后暂时阻止该窗口自动放大
	HWND _hwndChecked = NULL;
};

}
