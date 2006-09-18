#include "exiv2.hpp"

static void image_real_save(rbImage* image);

static void image_delete(rbImage* image) {
	__BEGIN
	image_real_save(image);
	delete image;
	__VOID_END
}

static void image_leave(rbImage* image) {
	
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


VALUE exiv2_image_clear(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	image->image->clearMetadata();
	image->dirty = true;
	return self;
	__END
}


VALUE exiv2_image_get_comment(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	std::string comment = image->image->comment();
	return rb_str_new(comment.c_str(), comment.length());
	__END
}

VALUE exiv2_image_set_comment(VALUE self, VALUE comment) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	switch(TYPE(comment)) {
		case T_STRING: {
			image->image->setComment(CSTR(comment));
			image->dirty = true;
			break;
		}
		case T_NIL: {
			image->image->clearComment();
			image->dirty = true;
			break;
		}
		default: {
			rb_raise(rb_eStandardError, "Can only set comment to string, or clear it with nil value");
		}
	}
	return comment;
	__END
}


VALUE exiv2_image_exif(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	VALUE exif = Data_Wrap_Struct(cExif, 0, image_leave, image);
	rb_iv_set(self, "image", self);
	return exif;
	__END
}


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
