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

