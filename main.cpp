#include <iostream>
#include "platform.h"
#include "DeckLinkAPI.h"

// settings for NTSC 29.97 - UYVY pixel format
#define BMD_DISPLAYMODE bmdModeNTSC
#define TCISDROPFRAME true
#define PIXEL_FMT bmdFormat8BitYUV

using namespace std;

std::string CFStringRefToString(CFStringRef cfString)
{
  if (cfString == nullptr)
    return "";

  // CFStringRefの長さを取得
  CFIndex length = CFStringGetLength(cfString) + 1;

  // char配列を確保
  char buffer[length];

  // CFStringRefをC文字列に変換
  if (CFStringGetCString(cfString, buffer, length, kCFStringEncodingUTF8))
  {
    return std::string(buffer);
  }
  else
  {
    // 変換に失敗した場合の処理
    return "";
  }
}

int main(int argc, char *argv[])
{
  HRESULT result;
  IDeckLinkIterator *deckLinkIterator;
  IDeckLink *deckLink;
  IDeckLinkInput *deckLinkInput = NULL;
  IDeckLinkProfileAttributes *deckLinkAttributes = NULL;
  IDeckLinkDisplayModeIterator *displayModeIterator = NULL;
  IDeckLinkDisplayMode *deckLinkDisplayMode = NULL;
  int numDevices = 0;

  // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
  result = GetDeckLinkIterator(&deckLinkIterator);
  if (result != S_OK)
  {
    fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
    return 1;
  }

  // Enumerate all cards in this system
  while (deckLinkIterator->Next(&deckLink) == S_OK)
  {
    CFStringRef deviceNameString;
    bool isRecorder = false;

    // Increment the total number of DeckLink cards found
    numDevices++;
    if (numDevices > 1)
      printf("\n\n");

    // *** Print the model name of the DeckLink card
    result = deckLink->GetDisplayName(&deviceNameString);
    if (result == S_OK)
    {
      std::string deviceName = CFStringRefToString(deviceNameString);
      printf("=============== %s ===============\n\n", deviceName.c_str());
      free((void *)deviceNameString);

      if (deviceName.find("Recorder") != std::string::npos)
      {
        isRecorder = true;
      }
    }

    // Products with multiple subdevices might not be usable if a subdevice is inactive for the current profile
    bool showIOinfo = true;
    result = deckLink->QueryInterface(IID_IDeckLinkProfileAttributes, (void **)&deckLinkAttributes);
    if (result != S_OK)
    {
      fprintf(stderr, "Could not obtain the IDeckLinkProfileAttributes interface - result = %08x\n", result);
      continue;
    }

    int64_t duplexMode;
    if (deckLinkAttributes->GetInt(BMDDeckLinkDuplex, &duplexMode) == S_OK && duplexMode == bmdDuplexInactive)
    {
      printf("Sub-device has no active connectors for current profile\n\n");
      showIOinfo = false;
    }

    int64_t videoIOSupport;
    result = deckLinkAttributes->GetInt(BMDDeckLinkVideoIOSupport, &videoIOSupport);
    if (result != S_OK)
    {
      fprintf(stderr, "Could not get BMDDeckLinkVideoIOSupport attribute - result = %08x\n", result);
      continue;
    }

    deckLinkAttributes->Release();

    // レコーダーかどうかを表示
    if (isRecorder)
    {
      // Start the capture
      cout << "This is a recorder" << endl;

      // Query the DeckLink for its input interface
      result = deckLink->QueryInterface(IID_IDeckLinkInput, (void **)&deckLinkInput);
      if (result != S_OK)
      {
        fprintf(stderr, "Could not obtain the IDeckLinkInput interface - result = %08x\n", result);
        return result;
      }

      if (deckLinkInput->GetDisplayModeIterator(&displayModeIterator) != S_OK)
      {
        fprintf(stderr, "Could not obtain the IDeckLinkDisplayModeIterator interface\n");
        return E_FAIL;
      }

      while (displayModeIterator->Next(&deckLinkDisplayMode) == S_OK)
      {
        if (deckLinkDisplayMode->GetDisplayMode() == BMD_DISPLAYMODE)
        {
          int width = deckLinkDisplayMode->GetWidth();
          int height = deckLinkDisplayMode->GetHeight();

          cout << "Width: " << width << endl;
          cout << "Height: " << height << endl;

          // deckLinkDisplayMode->GetFrameRate(&m_frameDuration, &m_timeScale);
          deckLinkDisplayMode->Release();
          deckLinkDisplayMode = NULL;

          break;
        }

        deckLinkDisplayMode->Release();
        deckLinkDisplayMode = NULL;
      }

      // enable video input
      if (deckLinkInput->EnableVideoInput(BMD_DISPLAYMODE, PIXEL_FMT, bmdVideoInputFlagDefault) != S_OK)
      {
        printf("Could not enable video input\n");
        return E_FAIL;
      }

      // start streaming
      if (deckLinkInput->StartStreams() != S_OK)
      {
        printf("Could not start streams\n");
        return E_FAIL;
      }
    }

    // Release the resources
    if (deckLinkInput != nullptr)
    {
      deckLinkInput->Release();
    }

    // Release the IDeckLink instance when we've finished with it to prevent leaks
    deckLink->Release();
  }

  cout << "Hello, Blackmagic decklink!" << endl;

  deckLinkIterator->Release();

  return 0;
}
