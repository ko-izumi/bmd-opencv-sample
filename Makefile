CC=g++
SDK_PATH=./include
CFLAGS=-std=c++11 -Wno-multichar -I $(SDK_PATH) -fno-rtti
LDFLAGS=-lm -ldl -lpthread

bmd-opencv-sample: main.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp
	$(CC) -o bmd-opencv-sample main.cpp platform.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp -framework CoreFoundation ${CFLAGS} ${LDFLAGS}

clean:
	rm -f DeviceList