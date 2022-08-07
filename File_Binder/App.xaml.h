#pragma once

#include "App.xaml.g.h"

namespace winrt::File_Binder::implementation
{
	struct App : AppT<App>
	{
		App();

		void OnLaunched(Microsoft::UI::Xaml::LaunchActivatedEventArgs const& args);

	private:
		winrt::Microsoft::UI::Xaml::Window window = nullptr;
	};
}
