SRCDIR=src
DATADIR=data
BUILDDIR?=build
GENERATOR?=Unix Makefiles
MINGW_BUILDDIR?=build-mingw
MINGW_INSTALLDIR?=windows
CMARK=$(BUILDDIR)/src/cmark
VERSION?=0.31.0
RELEASE?=cmark-$(VERSION)
INSTALL_PREFIX?=/usr/local
CLANG_CHECK?=clang-check
CLANG_FORMAT=clang-format -style llvm -sort-includes=0 -i

.PHONY: all cmake_build clean debug ubsan asan prof mingw archive format lint install uninstall distclean

all: cmake_build man/man3/cmark.3

$(CMARK): cmake_build

cmake_build: $(BUILDDIR)
	@$(MAKE) -j2 -C $(BUILDDIR)
	@echo "Binaries can be found in $(BUILDDIR)/src"

$(BUILDDIR):
	@cmake --version > /dev/null || (echo "You need cmake to build this program: http://www.cmake.org/download/" && exit 1)
	mkdir -p $(BUILDDIR); \
	cd $(BUILDDIR); \
	cmake .. \
		-G "$(GENERATOR)" \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-DCMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX) \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
		-DBUILD_SHARED_LIBS=YES

install: $(BUILDDIR)
	$(MAKE) -C $(BUILDDIR) install

uninstall: $(BUILDDIR)/install_manifest.txt
	xargs rm < $<

debug:
	mkdir -p $(BUILDDIR); \
	cd $(BUILDDIR); \
	cmake .. -DCMAKE_BUILD_TYPE=Debug; \
	$(MAKE)

ubsan:
	mkdir -p $(BUILDDIR); \
	cd $(BUILDDIR); \
	cmake .. -DCMAKE_BUILD_TYPE=Ubsan; \
	$(MAKE)

asan:
	mkdir -p $(BUILDDIR); \
	cd $(BUILDDIR); \
	cmake .. -DCMAKE_BUILD_TYPE=Asan; \
	$(MAKE)

prof:
	mkdir -p $(BUILDDIR); \
	cd $(BUILDDIR); \
	cmake .. -DCMAKE_BUILD_TYPE=Profile; \
	$(MAKE)

lint: $(BUILDDIR)
	errs=0 ; \
	for f in `ls src/*.[ch] | grep -v "scanners.c"` ; \
	  do echo $$f ; clang-tidy -header-filter='^build/.*' -p=build -warnings-as-errors='*' $$f || errs=1 ; done ; \
	exit $$errs

mingw:
	mkdir -p $(MINGW_BUILDDIR); \
	cd $(MINGW_BUILDDIR); \
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-mingw32.cmake -DCMAKE_INSTALL_PREFIX=$(MINGW_INSTALLDIR) ;\
	$(MAKE) && $(MAKE) install

man/man3/cmark.3: src/cmark.h | $(CMARK)
	python man/make_man_page.py $< > $@ \

archive:
	git archive --prefix=$(RELEASE)/ -o $(RELEASE).tar.gz HEAD
	git archive --prefix=$(RELEASE)/ -o $(RELEASE).zip HEAD

clean:
	rm -rf $(BUILDDIR) $(MINGW_BUILDDIR) $(MINGW_INSTALLDIR)

# We include case_fold_switch.inc in the repository, so this shouldn't
# normally need to be generated.
$(SRCDIR)/case_fold_switch.inc: $(DATADIR)/CaseFolding.txt
	perl tools/mkcasefold.pl < $< > $@

# We include scanners.c in the repository, so this shouldn't
# normally need to be generated.
$(SRCDIR)/scanners.c: $(SRCDIR)/scanners.re
	@case "$$(re2c -v)" in \
	    *\ 0.13.*|*\ 0.14|*\ 0.14.1) \
		echo "re2c >= 0.14.2 is required"; \
		false; \
		;; \
	esac
	re2c -W -Werror --case-insensitive -b -i --no-generation-date -8 \
		--encoding-policy substitute -o $@ $<
	$(CLANG_FORMAT) $@

# We include entities.inc in the repository, so normally this
# doesn't need to be regenerated:
$(SRCDIR)/entities.inc: tools/make_entities_inc.py
	python3 $< > $@

format:
	$(CLANG_FORMAT) src/*.c src/*.h

distclean: clean
	-rm -rf *.dSYM
	-rm -f README.html
