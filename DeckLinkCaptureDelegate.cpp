#include "DeckLinkCaptureDelegate.h"
#include <dispatch/dispatch.h>

// グローバルキューを定義
dispatch_queue_t mainQueue = dispatch_get_main_queue();

using namespace std;

HRESULT STDMETHODCALLTYPE DeckLinkCaptureDelegate::QueryInterface(REFIID iid, LPVOID *ppv)
{
  if (ppv == nullptr)
    return E_POINTER;

  *ppv = nullptr;
  return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE DeckLinkCaptureDelegate::AddRef(void)
{
  return ++refCount;
}

ULONG STDMETHODCALLTYPE DeckLinkCaptureDelegate::Release(void)
{
  ULONG newRefValue = --refCount;
  if (newRefValue == 0)
    delete this;
  return newRefValue;
}

HRESULT STDMETHODCALLTYPE DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioPacket)
{
  if (videoFrame)
  {
    // void *frameBytes;
    // videoFrame->GetBytes(&frameBytes);
    void *data;
    videoFrame->GetBytes(&data);

    const auto width = (int32_t)videoFrame->GetWidth();
    const auto height = (int32_t)videoFrame->GetHeight();
    const auto numBytes = videoFrame->GetRowBytes() * height;

    auto startTime = std::chrono::high_resolution_clock::now();

    cv::Mat frameCPU(height, width, CV_8UC2, data);
    cv::UMat image(height, width, CV_8UC1);

    cvtColor(frameCPU, image, cv::COLOR_YUV2GRAY_UYVY);

    // Convert to OpenCV Mat
    // cv::Mat img(videoFrame->GetHeight(), videoFrame->GetWidth(), CV_8UC4, frameBytes);

    // メインスレッドで実行するためにディスパッチキューを使用
    // dispatch_async(mainQueue, ^{
    //   cout << "Saving image" << endl;
    //   cv::imshow("Display window", frameCPU);
    //   cv::waitKey(1); // 必要に応じて待機時間を調整
    // });

    // 保存したい
    cout << "Saving image" << endl;
    // cv::imshow("Display window", img);
    // cv::imwrite("output.png", img);
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode *newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags)
{
  // Handle the format change event
  // For now, just return S_OK
  return S_OK;
}