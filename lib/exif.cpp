#include "exiv2.hpp"

VALUE exiv2_exif_get(VALUE self, VALUE key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::ExifData &exifData = image->image->exifData();
	
	if(exifData.empty()) {
		return Qnil;
	}
	
    Exiv2::ExifKey exifKey(STR(strkey));
    Exiv2::ExifData::const_iterator pos = exifData.findKey(exifKey);
    if (pos == exifData.end()) {
		return Qnil;
    }
	
	std::string v = pos->toString();
	return rb_str_new(v.c_str(), v.length());
	__NIL_END
}

VALUE exiv2_exif_set(VALUE self, VALUE key, VALUE value) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	VALUE strvalue = rb_funcall(value, rb_intern("to_s"), 0);
	Exiv2::ExifData &exifData = image->image->exifData();

	exifData[STR(strkey)] = STR(strvalue);
	image->dirty = true;
	return strvalue;
	__NIL_END
}

VALUE exiv2_exif_each(int argc, VALUE *argv, VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE prefix;
	rb_scan_args(argc, argv, "01", &prefix);
	
	Exiv2::ExifData &exifData = image->image->exifData();
	if(exifData.empty()) {
		return self;
	}
	
	Exiv2::ExifData::const_iterator end = exifData.end();
	for(Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
		VALUE key = rb_str_new(i->key().c_str(), i->key().length());
		VALUE val = rb_str_new(i->toString().c_str(), i->toString().length());
		if(prefix != Qnil && INT2FIX(0) != rb_funcall(key, rb_intern("index"), 1, prefix)) {
			continue;
		}
		rb_yield(rb_ary_new3(2, key, val));
	}
	return self;
	__END
}

VALUE exiv2_exif_delete(VALUE self, VALUE key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::ExifData &exifData = image->image->exifData();
	
	if(exifData.empty()) {
		return Qnil;
	}
	
    Exiv2::ExifKey exifKey(STR(strkey));
    Exiv2::ExifData::const_iterator pos = exifData.findKey(exifKey);
    if (pos == exifData.end()) {
		return Qnil;
    }
	
	std::string v = pos->toString();
	exifData.erase(pos);
	return rb_str_new(v.c_str(), v.length());
	__NIL_END
}

VALUE exiv2_exif_clear(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	
	if(exifData.empty()) {
		return Qnil;
	}
	exifData.clear();
	return self;
	__END
}


VALUE exiv2_exif_count(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	
	return INT2FIX(exifData.count());
	__END
}

VALUE exiv2_exif_empty(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	
	return exifData.empty() ? Qtrue : Qfalse;
	__NIL_END
}