#include "exiv2.hpp"


static VALUE unmarshall_value(const Exiv2::Value& value) {
	if(value.count() <= 0) {
		rb_warn("Empty value (no entries)");
		return Qnil;
	}
	
	Exiv2::TypeId type_id = value.typeId();
	switch(type_id) {
		case Exiv2::invalidTypeId: 
		{
			rb_warn("Trying to demarshall invalid type id");
			return Qnil;
		}
		
		case Exiv2::unsignedByte:
		case Exiv2::unsignedShort:
		case Exiv2::unsignedLong:
		case Exiv2::signedShort:
		case Exiv2::signedLong: 
		{
			return INT2NUM(value.toLong());
		}
		
		case Exiv2::asciiString:
		case Exiv2::string: 
		case Exiv2::undefined:
		{
			VALUE str = rb_str_buf_new(value.size() - 1);
			value.copy((Exiv2::byte *)STR(str), Exiv2::littleEndian);
			LEN(str) = value.size() - 1;
			return str;
		}
		
		case Exiv2::unsignedRational:
		case Exiv2::signedRational: 
		{
			return rb_str_new2(value.toString().c_str());
			Exiv2::Rational r = value.toRational();
			ID rational_id = rb_intern("Rational");
			if(0 && rb_const_defined(rb_cObject, rational_id)) {
				VALUE rational = rb_const_get(rb_cObject, rational_id);
				return rb_funcall(rational, rb_intern("new!"), 2, INT2NUM(r.first), INT2NUM(r.second));
			}
			return INT2NUM(r.first/r.second);
		}
		
		case Exiv2::invalid6:
		case Exiv2::date:
		case Exiv2::time:
		case Exiv2::comment:
		case Exiv2::directory:
		case Exiv2::lastTypeId:
		{
			VALUE str = rb_str_buf_new(value.size() - 1);
			value.copy((Exiv2::byte *)STR(str), Exiv2::littleEndian);
			LEN(str) = value.size() - 1;
			char msg[200];
			snprintf(msg, sizeof(msg), " !!! %d", type_id);
			rb_funcall(str, rb_intern("<<"), 1, rb_str_new2(msg));
			return str;
		}
	}
	return Qfalse;
}

/*
 * Access exif tag by name
 *
 * <code>Exiv2::Image.new("a.jpg").exif["Exif.Image.Model"] => "FinePixS2Pro"</code>
 */
static VALUE exiv2_exif_get(VALUE self, VALUE key) {
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
	
	return unmarshall_value(pos->value());
	//std::string v = pos->toString();
	//return rb_str_new(v.c_str(), v.length());
	__NIL_END
}

/*
 *  @exif["Exif.Photo.PixelXDimension"] = 3024
 * [] — is a universal accessor
 */
static VALUE exiv2_exif_set(VALUE self, VALUE key, VALUE value) {
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

/*
 * Iterates through all exif tags in image
 */
static VALUE exiv2_exif_each(int argc, VALUE *argv, VALUE self) {
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
		VALUE val = unmarshall_value(i->value());
		//VALUE val = rb_str_new(i->toString().c_str(), i->toString().length());
		if(prefix != Qnil && INT2FIX(0) != rb_funcall(key, rb_intern("index"), 1, prefix)) {
			continue;
		}
		rb_yield(rb_ary_new3(2, key, val));
	}
	return self;
	__END
}

/*
 * Delete exif value by it's name
 */
static VALUE exiv2_exif_delete(VALUE self, VALUE key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);
	Exiv2::ExifData &exifData = image->image->exifData();
	
	if(exifData.empty()) {
		return Qnil;
	}
	
    Exiv2::ExifKey exifKey(STR(strkey));
    Exiv2::ExifData::iterator pos = exifData.findKey(exifKey);
    if (pos == exifData.end()) {
		return Qnil;
    }
	
	std::string v = pos->toString();
	exifData.erase(pos);
	return rb_str_new(v.c_str(), v.length());
	__NIL_END
}

/*
 * Clear all exif data in image
 */
static VALUE exiv2_exif_clear(VALUE self) {
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

/*
 * Count of exif tags in image
 */
static VALUE exiv2_exif_count(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	
	return INT2FIX(exifData.count());
	__END
}

/*
 * Predicate method. Is exif empty?
 */
static VALUE exiv2_exif_empty(VALUE self) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::ExifData &exifData = image->image->exifData();
	
	return exifData.empty() ? Qtrue : Qfalse;
	__NIL_END
}



void Init_exif() {
	cExif = rb_define_class_under(mExiv2, "Exif", rb_cObject);
	rb_define_method(cExif, "each", VALUEFUNC(exiv2_exif_each), -1);
	rb_define_method(cExif, "[]", VALUEFUNC(exiv2_exif_get), 1);
	rb_define_method(cExif, "[]=", VALUEFUNC(exiv2_exif_set), 2);
	rb_define_method(cExif, "delete", VALUEFUNC(exiv2_exif_delete), 1);
	rb_define_method(cExif, "clear", VALUEFUNC(exiv2_exif_clear), 0);
	rb_define_method(cExif, "count", VALUEFUNC(exiv2_exif_count), 0);
	rb_define_method(cExif, "empty?", VALUEFUNC(exiv2_exif_empty), 0);
}