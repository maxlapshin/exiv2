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


VALUE mExiv2, cImage;

#ifdef __cplusplus
extern "C"
#end


void Init_exiv2() {
	mExiv2 = rb_define_module("Exiv2");
	
	cImage = rb_define_class_under(mExiv2, "Image", rb_cObject);
	rb_define_alloc_func(cImage, exiv2_image_s_allocate);
	rb_define_method(cImage, "initialize", VALUEFUNC(exiv2_image_initialize), 1);
	
}