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

  File:      exiv2.hpp
  Version:   $Rev: 1 $
  Author(s): Max Lapshin <max@maxidoors.ru>
 */

#ifndef EXIV2_HPP_
#define EXIV2_HPP_

// *****************************************************************************
// included header files
#include <image.hpp>
#include <exif.hpp>


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cassert>



#include "ruby.h"
#include "rubyio.h"
#ifdef __cplusplus
#  ifndef RUBY_METHOD_FUNC /* These definitions should work for Ruby 1.4.6 */
#    define PROTECTFUNC(f) ((VALUE (*)()) f)
#    define VALUEFUNC(f) ((VALUE (*)()) f)
#    define VOIDFUNC(f)  ((void (*)()) f)
#  else
#    ifndef ANYARGS /* These definitions should work for Ruby 1.6 */
#      define PROTECTFUNC(f) ((VALUE (*)()) f)
#      define VALUEFUNC(f) ((VALUE (*)()) f)
#      define VOIDFUNC(f)  ((RUBY_DATA_FUNC) f)
#    else /* These definitions should work for Ruby 1.7+ */
#      define PROTECTFUNC(f) ((VALUE (*)(VALUE)) f)
#      define VALUEFUNC(f) ((VALUE (*)(ANYARGS)) f)
#      define VOIDFUNC(f)  ((RUBY_DATA_FUNC) f)
#    endif
#  endif
#else
#  define VALUEFUNC(f) (f)
#  define VOIDFUNC(f) (f)
#endif

#define STR(x) (RSTRING(x)->ptr)
#define SAFESTR(x) (x == Qnil ? NULL : RSTRING(x)->ptr)
#define CSTR(x) (const char *)(STR(x))
#define CBSTR(x) (const uint8_t *)(STR(x))
#define CSAFESTR(x) (const char *)(SAFESTR(x))
#define LEN(x) (RSTRING(x)->len)
#define RUN(x) if(!x) { return Qnil; }

#define __BEGIN try {
#define __END } catch(Exiv2::AnyError& e) { rb_raise(rb_eStandardError, "Error occured in exiv2 library: %s", e.what().c_str());}
#define __NIL_END } catch(Exiv2::AnyError& e) { return Qnil; }
#define __VOID_END } catch(Exiv2::AnyError& e) {}

#if defined(__cplusplus)
extern "C" {
#endif
	void Init_exiv2(void);
	extern VALUE mExiv2, cImage, cExif, cIptc;

#if defined(__cplusplus)
}  /* extern "C" { */
#endif


struct rbImage {
	bool dirty;
	Exiv2::Image::AutoPtr image;
};


VALUE exiv2_image_s_allocate(VALUE klass);
VALUE exiv2_image_initialize(VALUE self, VALUE file);
VALUE exiv2_image_save(VALUE self);
VALUE exiv2_image_clear(VALUE self);
VALUE exiv2_image_exif(VALUE self);
VALUE exiv2_image_get_comment(VALUE self);
VALUE exiv2_image_set_comment(VALUE self, VALUE comment);

VALUE exiv2_exif_get(VALUE self, VALUE key);
VALUE exiv2_exif_set(VALUE self, VALUE key, VALUE value);
VALUE exiv2_exif_each(int argc, VALUE *argv, VALUE self);
VALUE exiv2_exif_delete(VALUE self, VALUE key);
VALUE exiv2_exif_size(VALUE self);
VALUE exiv2_exif_empty(VALUE self);
VALUE exiv2_exif_clear(VALUE self);


#endif /* EXIV2_HPP_ */
