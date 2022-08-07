#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "Utility.hpp"

namespace winrt::File_Binder::implementation
{
	MainWindow::MainWindow()
	{
		InitializeComponent();

		// get the window handle
		hWnd = Utility::GetWindowHandle(this);

		// no resize and maximize button
		auto windowLong = ::GetWindowLong(hWnd, GWL_STYLE);
		::SetWindowLong(hWnd, GWL_STYLE, windowLong & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);

		// resize and set in middle of screen
		Utility::CenterWindow(hWnd, { 640, 480 });

	}

	void MainWindow::OnDragEnterGrid(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::DragEventArgs const& args
	) {
		// when the dragged items are in the parent grid we accept the drag and show the "Add" text
		args.AcceptedOperation(::winrt::Windows::ApplicationModel::DataTransfer::DataPackageOperation::Link);
		args.DragUIOverride().Caption(L"Add");
	}


	::winrt::Windows::Foundation::IAsyncAction MainWindow::OnDropGrid(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::DragEventArgs const& args
	) {
		// check if the dropped items contains files/folders
		if (args.DataView().Contains(::winrt::Windows::ApplicationModel::DataTransfer::StandardDataFormats::StorageItems())) {
			// get the files and folders
			const auto& items = co_await args.DataView().GetStorageItemsAsync();
			for (const auto& item : items) {
				// Add only the files
				if (const auto& file = item.try_as<::winrt::Windows::Storage::StorageFile>()) {
					listViewFiles().Items().Append(::winrt::box_value(file.Path()));
				}
			}
		}

		// hide the placeholder of the list view
		if (listViewFiles().Items().Size()) {
			textBlockListViewFilesPlaceholder().Visibility(::winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
		}
	}

	void MainWindow::OnClickButtonBind(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/
	) {
		// convert the file paths from hstring to string
		::std::vector<::std::string> files;
		for (const auto& fileII : listViewFiles().Items()) {
			const auto& fileH = fileII.try_as<::winrt::hstring>();
			if (fileH) {
				files.push_back(::winrt::to_string(*fileH));
			}
		}

		// if there are no files we dont have anything to bind so return
		if (files.empty()) {
			return;
		}

		// if there are files bind them and update the progress ring and reset the UI
		// (do all of this in a separate thread so the UI don't freeze)
		const auto& pathSaveAs = ::winrt::to_string(textBlockPathSaveAs().Text());
		const auto& pathOpener = ::winrt::to_string(textBlockPathOpener().Text());
		::std::thread([&, pathSaveAs, pathOpener, files] {
			Utility::Bind(
				files,
				pathSaveAs,
				pathOpener,
				[&, fileCount = files.size()](::std::uint8_t fileBindedCount) {
				// because the progress ring is a part of UI and we update it
				// needs to run on the main thread (UI thread) so here we go
				DispatcherQueue().TryEnqueue([=] {
					progressRing().Value((float)fileBindedCount / fileCount * 100.f);
				});
			}
			);
		}).detach();
		Reset();
	}

	::winrt::Windows::Foundation::IAsyncAction MainWindow::OnClickButtonOpener(
		::winrt::Windows::Foundation::IInspectable const& sender,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
	) {
		// pick the file and update UI
		const auto& file = co_await Utility::PickFile(hWnd);
		if (!file.empty()) {
			textBlockPathOpener().Text(file);
		}
	}

	::winrt::Windows::Foundation::IAsyncAction MainWindow::OnClickButtonSaveAs(
		::winrt::Windows::Foundation::IInspectable const& sender,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& args
	) {
		// create picker and set it's parent to the window
		::winrt::Windows::Storage::Pickers::FileSavePicker picker;
		picker.as<IInitializeWithWindow>()->Initialize(hWnd);

		// set the directory to the desktop and suggest the file name as 'output'
		picker.SuggestedStartLocation(::winrt::Windows::Storage::Pickers::PickerLocationId::Desktop);
		picker.SuggestedFileName(L"output");

		// set the accepted file extensions
		const auto& extensions = ::winrt::single_threaded_vector<::winrt::hstring>({ L".exe" });
		picker.FileTypeChoices().Insert(L"Executable file", extensions);

		// pick the file and update UI
		const auto& file = co_await picker.PickSaveFileAsync();
		if (file) {
			textBlockPathSaveAs().Text(file.Path());
		}
	}

	void MainWindow::Reset() {
		// delete all the files paths, show the the list of paths placeholder
		listViewFiles().Items().Clear();
		textBlockListViewFilesPlaceholder().Visibility(::winrt::Microsoft::UI::Xaml::Visibility::Visible);

		// and remove the opener and clear the opener and output file paths
		textBlockPathOpener().Text(L"");
		textBlockPathSaveAs().Text(L"");

		// reset the progress ring value
		progressRing().Value(0);
	}
}
