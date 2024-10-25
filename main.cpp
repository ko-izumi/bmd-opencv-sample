#include <iostream>
#include "platform.h"
#include "DeckLinkAPI.h"

using namespace std;

int main(int argc, char *argv[])
{
  HRESULT result;
  IDeckLinkIterator *deckLinkIterator;
  int numDevices = 0;

  // Create an IDeckLinkIterator object to enumerate all DeckLink cards in the system
  result = GetDeckLinkIterator(&deckLinkIterator);
  if (result != S_OK)
  {
    fprintf(stderr, "A DeckLink iterator could not be created.  The DeckLink drivers may not be installed.\n");
    return 1;
  }

  cout << "Hello, Blackmagic decklink!" << endl;

  deckLinkIterator->Release();

  return 0;
}