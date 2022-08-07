#include "pch.h"

#include "App.xaml.h"
#include "MainWindow.xaml.h"

::winrt::File_Binder::implementation::App::App()
{
	InitializeComponent();

#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
	UnhandledException(
		[this](
			::winrt::Windows::Foundation::IInspectable const& /*sender*/,
			::winrt::Microsoft::UI::Xaml::UnhandledExceptionEventArgs const& /*args*/
			) {
		if (::IsDebuggerPresent()) {
			::__debugbreak();
		}
	}
	);
#endif
}

void ::winrt::File_Binder::implementation::App::OnLaunched(::winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const& /*args*/)
{
	window = ::winrt::make<::winrt::File_Binder::implementation::MainWindow>();
	window.Activate();
}