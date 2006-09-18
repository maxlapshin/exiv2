#include "exiv2.hpp"

static void image_real_save(rbImage* image);

static void image_delete(rbImage* image) {
	__BEGIN
	image_real_save(image);
	delete image;
	__VOID_END
}

VALUE exiv2_image_s_allocate(VALUE klass) {
	__BEGIN
	rbImage* image = new rbImage();
	image->dirty = false;
	return Data_Wrap_Struct(klass, 0, image_delete, image);
	__END
}

VALUE exiv2_image_initialize(VALUE self, VALUE file) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	
	
	try {
		if(rb_respond_to(file, rb_intern("read"))) {
			VALUE file_content = rb_funcall(file, rb_intern("read"), 0);
			rb_iv_set(self, "file_content", file_content);
			image->image = Exiv2::ImageFactory::open(CBSTR(file_content), LEN(file_content));
		} else if(TYPE(file) == T_STRING) {
			image->image = Exiv2::ImageFactory::open(CSTR(file));
		}
		image->image->readMetadata();
	}
	catch(const Exiv2::AnyError&) {
		rb_raise(rb_eStandardError, "Cannot open file %s", STR(file));
    }	
	return self;
	__END
}


VALUE exiv2_image_get_exif(VALUE self, VALUE key) {
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

VALUE exiv2_image_set_exif(VALUE self, VALUE key, VALUE value) {
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

static void image_real_save(rbImage* image) {
	if(image->dirty) {
		image->image->writeMetadata();
		image->dirty = false;
	}
}

VALUE exiv2_image_save(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	image_real_save(image);
	return self;
	__END
}