#pragma once

#include "pch.h"
#include "MainWindow.xaml.h"

#define SEPARATOR "*****"

class Utility
{
public:
	Utility() {}
	~Utility() {}

	static inline ::winrt::Windows::Foundation::IAsyncOperation<::winrt::Microsoft::UI::Xaml::Media::ImageSource>
		GetIconFromFile(
			const ::winrt::hstring& file,
			::winrt::Windows::Storage::FileProperties::ThumbnailMode thumbnailMode = ::winrt::Windows::Storage::FileProperties::ThumbnailMode::DocumentsView
		) {
		const auto& fileStorage = co_await ::winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(file);
		const auto& fileThumbnail = co_await fileStorage.GetThumbnailAsync(thumbnailMode);

		::winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmapImage;
		bitmapImage.SetSource(fileThumbnail);

		co_return bitmapImage;
	}

	// Gets the window handle associated with the MainWindow class
	static inline ::HWND GetWindowHandle(::winrt::File_Binder::implementation::MainWindow* mainWindow) {
		// get the native window and check the value
		const auto& iWindowNative = mainWindow->try_as<::IWindowNative>();
		::winrt::check_bool(iWindowNative);

		// get the handle window
		::HWND hWnd = nullptr;
		iWindowNative->get_WindowHandle(&hWnd);

		return hWnd;
	}

	// Resize and center the window
	static inline void CenterWindow(::HWND hWnd, const ::SIZE& size) {
		// get the desktop size
		auto hWndDesktop = ::GetDesktopWindow();
		::RECT hWndDesktopRect{};
		::GetWindowRect(hWndDesktop, &hWndDesktopRect);

		// move in the middle and resize window
		::SetWindowPos(
			hWnd,
			nullptr,
			(hWndDesktopRect.right - size.cx) / 2,
			(hWndDesktopRect.bottom - size.cy) / 2,
			size.cx,
			size.cy,
			0
		);
	}

	// Opens the default windows window for selecting a file
	static inline ::winrt::Windows::Foundation::IAsyncOperation<::winrt::hstring> PickFile(
		::HWND hWnd,
		::winrt::Windows::Storage::Pickers::PickerLocationId startLocation = ::winrt::Windows::Storage::Pickers::PickerLocationId::Desktop
	) {
		// create a picker and set it's father to the window
		::winrt::Windows::Storage::Pickers::FileOpenPicker picker;
		picker.as<::IInitializeWithWindow>()->Initialize(hWnd);

		// set the starting location and dont filter any files by extension
		picker.SuggestedStartLocation(startLocation);
		picker.FileTypeFilter().Append(L"*");

		// get the file path and return it
		const auto& storageFile = co_await picker.PickSingleFileAsync();
		co_return storageFile ? storageFile.Path() : L"";
	}

	// Finds all the occurrences of the 'query' in 'data' and populates the vector
	static void FindAllOccurrences(
		const ::std::string& data,
		const ::std::string& query,
		::std::vector<::size_t>& occurancesPoss
	) {
		// pretty straight forward
		::size_t pos = data.find(query);
		while (pos != ::std::string::npos) {
			occurancesPoss.push_back(pos);
			pos = data.find(query, pos + query.size());
		}
	}

	// Opens the specified file (as binary by default) and read all it's content into the 'str'
	static inline void FileAsString(
		const ::std::string& file,
		::std::string& str,
		const ::std::ios_base::openmode iosOM = ::std::ios::binary
	) {
		::std::ifstream ifs(file, iosOM);
		str.assign((::std::istreambuf_iterator<char>(ifs)), ::std::istreambuf_iterator<char>());
	}

	// Gets the unix timestamp in miliseconds
	static inline ::int64_t GetTimestampInMs() {
		// pretty straight forward
		const auto timestamp = ::std::chrono::system_clock::now().time_since_epoch();
		const auto timestampInMs = ::std::chrono::duration_cast<::std::chrono::milliseconds>(timestamp);

		return timestampInMs.count();
	}

	// Converts a string in a wide string
	static inline ::std::wstring StringToWString(const ::std::string& str) {
		return ::std::wstring(str.begin(), str.end());
	}

	// Binds all the 'files' and 'fileOpener' into 'fileBinded'
	static void Bind(
		const ::std::vector<::std::string>& files,
		const ::std::string& fileBinded,
		const ::std::string& fileOpener,
		const ::std::function<void(::std::uint8_t fileBindedCount)> callback
	) {
		// open the file as binary and write the opener first + separator
		::std::ofstream ofs(fileBinded, ::std::ios::binary);
		ofs << ::std::ifstream(fileOpener, ::std::ios::binary).rdbuf() << SEPARATOR;

		// write all the files + separators
		::size_t index = files.size();
		for (const auto& file : files) {
			ofs << ::std::ifstream(file, ::std::ios::binary).rdbuf();

			callback((uint8_t)(files.size() - index + 1));

			if (--index) {
				ofs << SEPARATOR;
			}
		}
	}

	// Gets the parent directory of the exe
	static inline ::std::string GetPathBase()
	{
		// get the exe path
		::std::string path(MAX_PATH, 0);
		::GetModuleFileNameA(nullptr, &path[0], MAX_PATH);

		// get exe's directory
		const auto pathBasePos = path.rfind('\\');
		return path.substr(0, pathBasePos) + '\\';
	}

	// Gets the app's assets folder
	static inline ::std::string GetPathAssets()
	{
		return GetPathBase() + "Assets\\";
	}

	// Prints a string between 2 new lines
	static inline void Print(const ::std::string& str) {
		::OutputDebugStringA("\n\n");
		::OutputDebugStringA(str.c_str());
		::OutputDebugStringA("\n\n");
	}
};