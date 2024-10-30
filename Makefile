CC=g++
SDK_PATH=./include
CFLAGS=-std=c++11 -Wno-multichar -I $(SDK_PATH) -frtti `pkg-config --cflags opencv4`
LDFLAGS=-lm -ldl -lpthread `pkg-config --libs opencv4`

bmd-opencv-sample: main.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp
	$(CC) -o bmd-opencv-sample main.cpp DeckLinkUtil.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp -framework CoreFoundation ${CFLAGS} ${LDFLAGS}

clean:
	rm -f DeviceList