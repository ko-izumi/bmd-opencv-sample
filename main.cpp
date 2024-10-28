#include <iostream>
#include "platform.h"
#include "DeckLinkAPI.h"

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
  IDeckLinkProfileAttributes *deckLinkAttributes = NULL;
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
      // レコーダーの場合、映像を取得する
      cout << "This is a recorder" << endl;
    }

    // Release the IDeckLink instance when we've finished with it to prevent leaks
    deckLink->Release();
  }

  cout << "Hello, Blackmagic decklink!" << endl;

  deckLinkIterator->Release();

  return 0;
}
