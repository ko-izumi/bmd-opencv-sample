#ifndef DECKLINKCAPTUREDELEGATE_H
#define DECKLINKCAPTUREDELEGATE_H

#include <DeckLinkAPI.h>
#include <opencv2/opencv.hpp>

// グローバルキューを作成
extern dispatch_queue_t mainQueue;

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
{
public:
  DeckLinkCaptureDelegate() : refCount(1) {}
  virtual ~DeckLinkCaptureDelegate() {}

  // IUnknown methods
  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) override;
  virtual ULONG STDMETHODCALLTYPE AddRef(void) override;
  virtual ULONG STDMETHODCALLTYPE Release(void) override;

  // IDeckLinkInputCallback methods
  virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame *videoFrame, IDeckLinkAudioInputPacket *audioPacket) override;
  virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents notificationEvents, IDeckLinkDisplayMode *newDisplayMode, BMDDetectedVideoInputFormatFlags detectedSignalFlags) override;

private:
  ULONG refCount;
};

#endif // DECKLINKCAPTUREDELEGATE_H