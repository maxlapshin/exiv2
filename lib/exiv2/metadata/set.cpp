static VALUE exiv2_metadata_set(VALUE key, VALUE value) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(key, rb_intern("to_s"), 0);

	if(!marshall_value(image->image-><%= kind.downcase %>Data(), STR(strkey), value)) {
		THROW("Couldn't write %s", STR(strkey));
	}
	
	image->dirty = true;
	return value;
	__NIL_END
	
}
