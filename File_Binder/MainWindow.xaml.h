#pragma once

#include "MainWindow.g.h"

namespace winrt::File_Binder::implementation
{
	struct MainWindow : MainWindowT<MainWindow>
	{
		MainWindow();

		void OnClickButtonClearAll(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

		void OnClickButtonReset(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

		void OnDragEnterGrid(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::DragEventArgs const& args
		);

		::winrt::Windows::Foundation::IAsyncAction OnDropGrid(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::DragEventArgs const& args
		);

		::winrt::Windows::Foundation::IAsyncAction OnClickButtonSaveAs(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

		::winrt::Windows::Foundation::IAsyncAction OnClickButtonOpener(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

		void OnClickButtonBind(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

		void OnClickListViewItemButtonRemove(
			::winrt::Windows::Foundation::IInspectable const& sender,
			::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
		);

	private:
		::winrt::Windows::Foundation::IAsyncAction AddItemToListView(
			::winrt::File_Binder::implementation::MainWindow* mainWindow,
			::winrt::hstring filePath
		);

		void Reset();

		HWND hWnd = nullptr;
	};
}

namespace winrt::File_Binder::factory_implementation
{
	struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
	{
	};
}
