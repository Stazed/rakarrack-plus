message("In /doc/help/css/")

#/usr/bin/install -c -m 644 kde-default.css '/usr/local/share/doc/rakarrack/html/css'

install (FILES kde-default.css
    DESTINATION share/doc/rakarrack/html/css)

#htmldir = $(docdir)/html/css
#html_DATA= kde-default.css
	
#EXTRA_DIST = $(html_DATA)