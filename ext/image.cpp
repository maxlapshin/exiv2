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

/*
 * Document-class: Exiv2::Image
 * Image is used to access to all exif data of image
 *
 *   @image = Exiv2::Image.new("my.jpg")
 *   puts @image["Iptc.Application2.DateCreated"]
 *   puts @image["Exif.Image.Software"]
 */
static VALUE exiv2_image_s_allocate(VALUE klass) {
	__BEGIN
	rbImage* image = new rbImage();
	image->dirty = false;
	return Data_Wrap_Struct(klass, 0, image_delete, image);
	__END
}

/*
 *   img = Exiv2::Image.new("IMGP3025.jpg")
 */
static VALUE exiv2_image_initialize(VALUE self, VALUE file) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	
	
	try {
		if(rb_respond_to(file, rb_intern("read"))) {
			VALUE file_content = rb_funcall(file, rb_intern("read"), 0);
			rb_iv_set(self, "@file_content", file_content);
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

/*
 *
 * Load Exiv2::Image from memory string
 *
 *   content = File.open("a.jpg").read
 *   img = Exiv2::Image.load_string(content)
 */
static VALUE exiv2_image_load_string(VALUE self, VALUE string) {
	__BEGIN
	Check_Type(string, T_STRING);
	rbImage* image = new rbImage();
	image->dirty = false;
	VALUE img = Data_Wrap_Struct(self, 0, image_delete, image);
	
	image->image = Exiv2::ImageFactory::open(CBSTR(string), LEN(string));
	return img;
	__END
}


static void image_real_save(rbImage* image) {
	if(image->dirty) {
		image->image->writeMetadata();
		image->dirty = false;
	}
}

/*
 * Save image with changed data
 */
static VALUE exiv2_image_save(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	image_real_save(image);
	return self;
	__END
}

/*
 * Clear all metadata in image. Not only exif
 */
static VALUE exiv2_image_clear(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	image->image->clearMetadata();
	image->dirty = true;
	return self;
	__END
}

/*
 * Get comment of image
 */
static VALUE exiv2_image_get_comment(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	std::string comment = image->image->comment();
	return rb_str_new(comment.c_str(), comment.length());
	__END
}

/*
 * Set comment in image
 */
static VALUE exiv2_image_set_comment(VALUE self, VALUE comment) {
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

/*
 * Access to exif data of image
 */
static VALUE exiv2_image_exif(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	VALUE exif = Data_Wrap_Struct(cExif, 0, image_leave, image);
	rb_iv_set(exif, "@image", self);
	return exif;
	__END
}

/*
 * Access to iptc data of image
 */
static VALUE exiv2_image_iptc(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);
	VALUE iptc = Data_Wrap_Struct(cIptc, 0, image_leave, image);
	rb_iv_set(iptc, "@image", self);
	return iptc;
	__END
}


/*
 * Dump thumbnail to file.
 * @img.thumbnail("my_image")
 */
static VALUE exiv2_image_thumbnail(VALUE self, VALUE file_name) {
	__BEGIN
	Check_Type(file_name, T_STRING);
	
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	exifData.writeThumbnail(STR(file_name));
	if(rb_block_given_p()) {
		rb_yield(file_name);
	}
	return self;
	__END
}

/*
 * Set image thumbnail to contents of passed file
 * @img.thumbnail = "my_image.jpg"
 */
static VALUE exiv2_image_thumbnail_set(VALUE self, VALUE file_name) {
	__BEGIN
	Check_Type(file_name, T_STRING);
	
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	exifData.setJpegThumbnail(STR(file_name));
	return self;
	__END
}


void Init_image() {
	cImage = rb_define_class_under(mExiv2, "Image", rb_cObject);
	rb_define_alloc_func(cImage, exiv2_image_s_allocate);
	rb_define_method(cImage, "initialize", VALUEFUNC(exiv2_image_initialize), 1);
	rb_define_singleton_method(cImage, "load_string", VALUEFUNC(exiv2_image_load_string), 1);
	
	rb_define_method(cImage, "exif", VALUEFUNC(exiv2_image_exif), 0);
	rb_define_method(cImage, "Exif", VALUEFUNC(exiv2_image_exif), 0);
	rb_define_method(cImage, "iptc", VALUEFUNC(exiv2_image_iptc), 0);

	rb_define_method(cImage, "thumbnail", VALUEFUNC(exiv2_image_thumbnail), 1);
	rb_define_method(cImage, "thumbnail=", VALUEFUNC(exiv2_image_thumbnail_set), 1);

	rb_define_method(cImage, "save", VALUEFUNC(exiv2_image_save), 0);
	rb_define_method(cImage, "clear", VALUEFUNC(exiv2_image_clear), 0);
	rb_define_method(cImage, "comment", VALUEFUNC(exiv2_image_get_comment), 0);
	rb_define_method(cImage, "comment=", VALUEFUNC(exiv2_image_set_comment), 1);
}
