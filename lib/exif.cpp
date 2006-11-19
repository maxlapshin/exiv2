#include "exiv2.hpp"




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
	Exiv2::ExifData &exifData = image->image->exifData();

	if(!marshall_value(exifData, STR(strkey), value)) {
		THROW("Couldn't write %s", STR(strkey));
	}
	
	image->dirty = true;
	return value;
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
		return Qnil;
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



static void tag_leave(Exiv2::TagInfo* info) {
	
}

static VALUE create_exiv2_tag(Exiv2::TagInfo* info) {
	return Data_Wrap_Struct(cTag, 0, tag_leave, info);
}

static int iterate_tag_collection(const Exiv2::TagInfo* collection, bool to_yield = true) {
	Exiv2::TagInfo* _collection = const_cast<Exiv2::TagInfo *>(collection);
	int i;
    for (i=0; _collection[i].tag_ != 0xffff; ++i) {
		if(to_yield) {
			rb_yield(create_exiv2_tag(_collection + i));
		}
    }
	return i;
}

#ifdef HAVE_IFDTAGLIST
static VALUE exiv2_tags_each(VALUE self) {
	__BEGIN
	iterate_tag_collection(Exiv2::ExifTags::ifdTagList());
	iterate_tag_collection(Exiv2::ExifTags::exifTagList());
	iterate_tag_collection(Exiv2::ExifTags::iopTagList());
	iterate_tag_collection(Exiv2::ExifTags::gpsTagList());
	return self;
	__END
}



static VALUE exiv2_tags_count(VALUE self) {
	__BEGIN
	return INT2NUM(
	iterate_tag_collection(Exiv2::ExifTags::ifdTagList(), false) +
	iterate_tag_collection(Exiv2::ExifTags::exifTagList(), false) +
	iterate_tag_collection(Exiv2::ExifTags::iopTagList(), false) +
	iterate_tag_collection(Exiv2::ExifTags::gpsTagList(), false)
	);
	__END
}
#endif /* HAVE_IFDTAGLIST */

static VALUE exiv2_tag_name(VALUE self) {
	__BEGIN
	Exiv2::TagInfo* tag;
	Data_Get_Struct(self, Exiv2::TagInfo, tag);
	
	return tag->name_ ? rb_str_new2(tag->name_) : Qnil;
	__END
}

static VALUE exiv2_tag_title(VALUE self) {
	__BEGIN
	Exiv2::TagInfo* tag;
	Data_Get_Struct(self, Exiv2::TagInfo, tag);
	
	return tag->title_ ? rb_str_new2(tag->title_) : Qnil;
	__END
}

static VALUE exiv2_tag_desc(VALUE self) {
	__BEGIN
	Exiv2::TagInfo* tag;
	Data_Get_Struct(self, Exiv2::TagInfo, tag);
	
	return tag->desc_ ? rb_str_new2(tag->desc_) : Qnil;
	__END
}

static VALUE exiv2_tag_section(VALUE self) {
	__BEGIN
	Exiv2::TagInfo* tag;
	Data_Get_Struct(self, Exiv2::TagInfo, tag);
	
	return rb_str_new2(Exiv2::ExifTags::sectionName(tag->sectionId_));
	__END
}

static VALUE exiv2_tag_ifd(VALUE self) {
	__BEGIN
	Exiv2::TagInfo* tag;
	Data_Get_Struct(self, Exiv2::TagInfo, tag);
	
	return rb_str_new2(Exiv2::ExifTags::ifdName(tag->ifdId_));
	__END
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
//	rb_define_singleton_method(cExif, "iptc_tags_each", VALUEFUNC(exiv2_iptc_tags_each), 0);
	
	cTag = rb_define_class_under(mExiv2, "Tag", rb_cObject);
#ifdef HAVE_IFDTAGLIST
	rb_define_singleton_method(cTag, "each", VALUEFUNC(exiv2_tags_each), 0);
	rb_define_singleton_method(cTag, "count", VALUEFUNC(exiv2_tags_count), 0);
#endif /* HAVE_IFDTAGLIST */
	
	rb_define_method(cTag, "ifd", VALUEFUNC(exiv2_tag_ifd), 0);
	rb_define_method(cTag, "section", VALUEFUNC(exiv2_tag_section), 0);
	rb_define_method(cTag, "name", VALUEFUNC(exiv2_tag_name), 0);
	rb_define_method(cTag, "title", VALUEFUNC(exiv2_tag_title), 0);
	rb_define_method(cTag, "desc", VALUEFUNC(exiv2_tag_desc), 0);
}