#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include "Utility.hpp"

namespace winrt::File_Binder::implementation
{
	// A small wrapper to create a ColumnDefinition object
	::winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition CreateDefinitionColumn(
		const double width,
		::winrt::Microsoft::UI::Xaml::GridUnitType unitType = ::winrt::Microsoft::UI::Xaml::GridUnitType::Star
	) {
		::winrt::Microsoft::UI::Xaml::Controls::ColumnDefinition definitionColumn;
		definitionColumn.Width({ width, unitType });

		return definitionColumn;
	}

	// A small wrapper to create a Thickness object
	::winrt::Microsoft::UI::Xaml::Thickness CreateThickness(
		const double left = 0.,
		const double top = 0.,
		const double right = 0.,
		const double bottom = 0.
	) {
		::winrt::Microsoft::UI::Xaml::Thickness thickness{ left, top, right, bottom };
		return thickness;
	}

	// Get the file path from the list view item
	::winrt::hstring GetFilePath(
		::winrt::File_Binder::implementation::MainWindow* mainWindow,
		const uint32_t index
	) {
		// get, convert, get, convert, get, convert and finally get the text
		const auto& ii = mainWindow->listViewFiles().ContainerFromIndex(index);
		const auto& listViewItem = ii.try_as<::winrt::Microsoft::UI::Xaml::Controls::ListViewItem>();
		const auto& root = listViewItem.ContentTemplateRoot();
		const auto& grid = root.try_as<::winrt::Microsoft::UI::Xaml::Controls::Grid>();
		const auto& textBlockII = grid.Children().GetAt(1);
		const auto& textBlock = textBlockII.try_as<::winrt::Microsoft::UI::Xaml::Controls::TextBlock>();

		return textBlock.Text();
	}

	void MainWindow::OnClickButtonClearAll(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/
	) {
		listViewFiles().Items().Clear();
	}

	void MainWindow::OnClickButtonReset(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/
	) {
		Reset();
	}

	// Create a listviewitem and append it
	::winrt::Windows::Foundation::IAsyncAction MainWindow::AddItemToListView(
		::winrt::File_Binder::implementation::MainWindow* mainWindow,
		::winrt::hstring filePath
	) {
		// File icon
		::winrt::Microsoft::UI::Xaml::Controls::Image icon;
		// set size
		icon.Width(40);
		icon.Height(40);
		// set icon
		icon.Source(co_await Utility::GetIconFromFile(filePath));

		// create the text block with the file path
		::winrt::Microsoft::UI::Xaml::Controls::TextBlock path;
		// set the text block text and vertical alignment and add a small left padding
		path.Text(filePath);
		path.VerticalAlignment(::winrt::Microsoft::UI::Xaml::VerticalAlignment::Center);
		path.Padding(CreateThickness(10.));

		// create a button to remove the items from the list
		::winrt::Microsoft::UI::Xaml::Controls::Button remove;
		// set the on click event and align it to right
		remove.Click([&](const auto& sender, const auto& args) {
			OnClickListViewItemButtonRemove(sender, args);
		});

		// create an icon for the remove button and set it
		::winrt::Microsoft::UI::Xaml::Controls::FontIcon removeIcon;
		removeIcon.Glyph(L"\uE10A");
		remove.Content(removeIcon);

		// create a layout 
		::winrt::Microsoft::UI::Xaml::Controls::Grid layout;
		// set the column definitions of the layout
		layout.ColumnDefinitions().Append(CreateDefinitionColumn(10));
		layout.ColumnDefinitions().Append(CreateDefinitionColumn(80));
		layout.ColumnDefinitions().Append(CreateDefinitionColumn(10));
		// add the children to the layout
		layout.Children().Append(icon);

		layout.Children().Append(path);
		layout.SetColumn(path, 1);

		layout.Children().Append(remove);
		layout.SetColumn(remove, 2);

		// create the list view item and set it's content to out layout
		::winrt::Microsoft::UI::Xaml::Controls::ListViewItem item;
		item.Content(layout);

		// add the list view item to the list view
		mainWindow->listViewFiles().Items().Append(item);
	}

	MainWindow::MainWindow()
	{
		InitializeComponent();

		// get the window handle
		hWnd = Utility::GetWindowHandle(this);

		// no resize and maximize button
		const auto windowLong = ::GetWindowLong(hWnd, GWL_STYLE);
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
					AddItemToListView(this, file.Path());

					// hide the placeholder of the list view
					if (textBlockListViewFilesPlaceholder().Visibility() == ::winrt::Microsoft::UI::Xaml::Visibility::Visible) {
						textBlockListViewFilesPlaceholder().Visibility(::winrt::Microsoft::UI::Xaml::Visibility::Collapsed);
					}
				}
			}
		}
	}

	void MainWindow::OnClickButtonBind(
		::winrt::Windows::Foundation::IInspectable const& /*sender*/,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/
	) {
		// convert the file paths from hstring to string
		::std::vector<::std::string> files;
		for (uint32_t i = 0; i < listViewFiles().Items().Size(); i++) {
			const auto& fileH = GetFilePath(this, i);
			if (!fileH.empty()) {
				files.push_back(::winrt::to_string(fileH));
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
					progressBar().Value((double)fileBindedCount / fileCount * 100.);
					progressProcent().Text(to_hstring((uint8_t)progressBar().Value()) + L" %");
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

	void MainWindow::OnClickListViewItemButtonRemove(
		::winrt::Windows::Foundation::IInspectable const& sender,
		::winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*args*/
	) {
		auto lvi = sender
			.try_as<::winrt::Microsoft::UI::Xaml::Controls::Button>()
			.Parent()
			.try_as<::winrt::Microsoft::UI::Xaml::Controls::Grid>()
			.Parent()
			.try_as<::winrt::Microsoft::UI::Xaml::Controls::ListViewItem>();

		uint32_t lviIndex;
		listViewFiles().Items().IndexOf(lvi, lviIndex);

		listViewFiles().Items().RemoveAt(lviIndex);
	}

	void MainWindow::Reset() {
		// delete all the files paths, show the the list of paths placeholder
		listViewFiles().Items().Clear();
		textBlockListViewFilesPlaceholder().Visibility(::winrt::Microsoft::UI::Xaml::Visibility::Visible);

		// and remove the opener and clear the opener and output file paths
		textBlockPathSaveAs().Text(L"");
		/*textBlockPathOpener().Text(L"");*/

		// reset the progress bar and procent value
		progressBar().Value(0);
		progressProcent().Text(L"0 %");
	}
}