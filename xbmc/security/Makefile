SRCS=KeyringManager.cpp \
     MemoryKeyringManager.cpp \
     XMLKeyringManager.cpp \

LIB=security.a

include ../../Makefile.include
-include $(patsubst %.cpp,%.P,$(patsubst %.c,%.P,$(patsubst %.S,,$(SRCS))))
