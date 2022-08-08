#pragma once

// win32
#include <unknwn.h>
#include <hstring.h>
#include <windows.h>
#include <ShObjIdl_core.h>
#include <restrictederrorinfo.h>

#undef GetCurrentTime

// std
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// winrt-windows
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Pickers.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>

// winrt-microsoft
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Microsoft.UI.Xaml.Data.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.Markup.h>
#include <winrt/Microsoft.UI.Xaml.Shapes.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Composition.h>
#include <winrt/Microsoft.UI.Xaml.Interop.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Navigation.h>
#include <winrt/Microsoft.Ui.Xaml.Media.Imaging.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>

// wil
#include <wil/cppwinrt_helpers.h>

// random
#include <microsoft.ui.xaml.window.h>