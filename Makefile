# opt this out for your favorite compiler.
# favorites:
# - g++
# - clang++
COMPILER ?= g++
CFLAGS   ?= -O1 -g

# bootstrap file
CXX_MAIN ?= backend/binit/build.init.cpp

bpp: $(CXX_MAIN)
	$(COMPILER) $(CFLAGS) -o $@ $(CXX_MAIN)
