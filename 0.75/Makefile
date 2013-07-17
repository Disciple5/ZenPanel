# Makefile
# Copyright (C) 2006 Jean-Philippe Guillemin <jp.guillemin@free.fr>
# license: This software is under GPL version 2 of license

PROGRAM = zenpanel

DESTDIR =
PREFIX = /usr
DATAPATH = $(PREFIX)/share/systemtools
SBINPATH = $(PREFIX)/sbin
BINPATH = $(PREFIX)/bin
LOCALEPATH = $(PREFIX)/share/locale
LIBEXECPATH = $(PREFIX)/libexec
FREEDESKTOPPATH = $(PREFIX)/share/applications
ICONPATH = $(PREFIX)/share/icons/hicolor/scalable/apps
CATEGORY = X-Zenpanel
banner = banner.png
lib = zpnl_parser

GTK_CFLAGS = -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include  
GTK_LIBS = -lgtk-x11-2.0 -latk-1.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0  
CFLAGS = -Wall -O2 -march=i486 -mtune=i686

CFLAGS += $(GTK_CFLAGS)

BANNER = $(DATAPATH)/$(banner)
DEFINE = -D_REENTRANT -D_LOCALEPATH='"$(LOCALEPATH)"' -D_DEFAULT_DESKTOPPATH='"$(FREEDESKTOPPATH)"' -D_DEFAULT_BANNER='"$(BANNER)"' -D_DEFAULT_CATEGORY='"$(CATEGORY)"'

all : $(PROGRAM) $(PROGRAM).po

$(PROGRAM) : $(PROGRAM).o $(lib).o $(PROGRAM).h 
	gcc $(CFLAGS) $(GTK_LIBS) -o $@ $^

$(lib).o : $(lib).c 
	gcc $(CFLAGS) $(DEFINE) -c $^
	
$(PROGRAM).o : $(PROGRAM).c 
	gcc $(CFLAGS) $(DEFINE) -c $^

debug: DEFINE += -DDEBUG
debug: CFLAGS += -g
debug: $(PROGRAM)

$(PROGRAM).po: 
	xgettext -s --from-code=UTF-8 --language=C --keyword=_ -d $(PROGRAM) -o $(PROGRAM).po $(PROGRAM).c
	for PO in `ls po/*.po` ; do \
		msgmerge -U -N --previous $$PO $(PROGRAM).po; \
	done; 
	rm -f po/*.po~

install : $(PROGRAM) $(DESTDIR)$(BINPATH) $(DESTDIR)$(DATAPATH) $(DESTDIR)$(FREEDESKTOPPATH) $(DESTDIR)$(ICONPATH)
	strip $(PROGRAM)	
	install -m 0755 -o root -g root $(PROGRAM) $(DESTDIR)$(BINPATH)
	install -m 0644 -o root -g root $(banner) $(DESTDIR)$(DATAPATH)
	install -m 0644 -o root -g root $(PROGRAM).desktop $(DESTDIR)$(FREEDESKTOPPATH)
	install -m 0644 -o root -g root $(PROGRAM).svg $(DESTDIR)$(ICONPATH)
	for PO in `ls po/*.po` ; do \
		LANGUAGE=`basename $$PO .po`; \
		mkdir -p $(DESTDIR)$(LOCALEPATH)/$$LANGUAGE/LC_MESSAGES; \
		msgfmt -o $(DESTDIR)$(LOCALEPATH)/$$LANGUAGE/LC_MESSAGES/$(PROGRAM).mo -v $$PO; \
	done; 
	rm -f po/*.po~

uninstall :
	rm -f $(DESTDIR)$(BINPATH)/$(PROGRAM)
	rm -f $(DESTDIR)$(DATAPATH)/$(banner)
	rm -f $(DESTDIR)$(FREEDESKTOPPATH)/$(PROGRAM).desktop
	rm -f $(DESTDIR)$(ICONPATH)/$(PROGRAM).svg
	rm -f $(DESTDIR)$(LOCALEPATH)/*/LC_MESSAGES/$(PROGRAM).mo

clean :
	rm -f $(PROGRAM).o
	rm -f $(PROGRAM).po
	rm -f $(PROGRAM)
	rm -f $(lib).o

$(DESTDIR)$(BINPATH) :
	mkdir -p $(DESTDIR)$(BINPATH)

$(DESTDIR)$(DATAPATH) :
	mkdir -p $(DESTDIR)$(DATAPATH)

$(DESTDIR)$(FREEDESKTOPPATH) :
	mkdir -p $(DESTDIR)$(FREEDESKTOPPATH)
	
$(DESTDIR)$(ICONPATH) :
	mkdir -p $(DESTDIR)$(ICONPATH)
