// ***************************************************************** -*- C++ -*-
/*
 * Copyright (C) 2006 Max Lapshin <max@maxidoors.ru>
 *
 * This program is part of the ruby-exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*
  Abstract:  Driver for using exiv2 library (http://www.exiv2.org/) in ruby language

  File:      exiv2.cpp
  Version:   $Rev: 1 $
  Author(s): Max Lapshin <max@maxidoors.ru>
 */
// *****************************************************************************
#include "exiv2.hpp"


VALUE mExiv2, cImage, cExif, cTag, cIptc, cThumbnail, eError;

void rb_exiv2_throw(const char *file, long unsigned int line, const char *fmt, ...) {
	char* message;
	va_list ap;
	va_start(ap, fmt);
	vasprintf(&message, fmt, ap);
	va_end(ap);
	
	char error_message[strlen(message) + 80];
	snprintf(error_message, sizeof(error_message), "%s. File: %s, line: %lu", message, file, line);
	free(message);
	rb_raise(eError, error_message);
}


#ifdef __cplusplus
extern "C"
#endif



void Init_exiv2_bin() {
	mExiv2 = rb_define_module("Exiv2");
	
	eError = rb_define_class_under(mExiv2, "Error", rb_eStandardError);
	
	Init_image();
	Init_exif();
	Init_iptc();
	//cThumbnail = rb_define_class_under(mExiv2, "Thumbnail", rb_cObject);
	//rb_define_method(cThumbnail, "extension", VALUEFUNC(exiv2_thumb_ext), 0);
	//rb_define_method(cThumbnail, "format", VALUEFUNC(exiv2_thumb_format), 0);
	//rb_define_method(cThumbnail, "clear", VALUEFUNC(exiv2_thumb_clear), 0);
	//rb_define_method(cThumbnail, "to_s", VALUEFUNC(exiv2_thumb_read), 0);
	//rb_define_method(cThumbnail, "read", VALUEFUNC(exiv2_thumb_read), 0);
	//rb_define_method(cThumbnail, "write", VALUEFUNC(exiv2_thumb_write), 1);
}