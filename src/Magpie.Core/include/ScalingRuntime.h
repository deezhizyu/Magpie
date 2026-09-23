#pragma once
#include "Event.h"

namespace Magpie {

enum class ScalingState {
	Idle,
	Scaling,
	Waiting
};

// 用于 Auto-adjust 功能:读取当前缩放会话中未应用 outputScale 的输出尺寸
// 以及渲染器矩形(物理显示器/窗口区域),据此计算合适的 outputScale/outputOffset
struct OutputFitInfo {
	bool isScaling = false;
	SIZE unscaledSize{};
	RECT rendererRect{};
};

class ScalingRuntime {
public:
	ScalingRuntime();
	~ScalingRuntime();

	bool Start(HWND hwndSrc, struct ScalingOptions&& options, bool force);

	// 阻塞直至缩放线程返回结果，仅用于响应用户的一次性操作(如 Auto-adjust 按钮)
	OutputFitInfo GetOutputFitInfo() noexcept;

	void ToggleScaling(bool isWindowedMode);

	void SwitchToolbarState();

	void TakeScreenshot();

	void Stop();

	ScalingState State() const noexcept {
		return _state.load(std::memory_order_relaxed);
	}

	// 调用者应处理线程同步
	MultithreadEvent<ScalingState> StateChanged;

private:
	void _ScalingThreadProc() noexcept;

	// 确保 _dispatcher 完成初始化
	const winrt::DispatcherQueue& _Dispatcher() noexcept;

	void _State(ScalingState value);

	std::thread _scalingThread;

	winrt::DispatcherQueue _dispatcher{ nullptr };
	std::atomic<bool> _dispatcherInitialized = false;
	// 只能在主线程访问，省下检查 _dispatcherInitialized 的开销
	bool _dispatcherInitializedCache = false;

	std::atomic<ScalingState> _state = ScalingState::Idle;
};

}
