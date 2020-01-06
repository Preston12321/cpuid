CC=g++
CPPFLAGS=-m64 -g
ASFLAGS=-g
BUILDDIR=build

all: $(BUILDDIR)/cpuid

$(BUILDDIR)/cpuid: $(BUILDDIR)/cpuint.o cpuid.cpp cpuint.cpp | $(BUILDDIR)
	$(CC) -o $(BUILDDIR)/cpuid $(CPPFLAGS) $+

$(BUILDDIR)/%.o : %.s | $(BUILDDIR)
	as -o $@ $(ASFLAGS) $<

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	rm -rf build
