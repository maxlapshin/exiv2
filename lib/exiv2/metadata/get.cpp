static VALUE exiv2_metadata_get(VALUE rb_key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(rb_key, rb_intern("to_s"), 0);
	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();

	if(data.empty()) {
		return Qnil;
	}

  Exiv2::<%= kind %>Key key(STR(strkey));
  Exiv2::<%= kind %>Data::const_iterator pos = data.findKey(key);
  if (pos == data.end()) {
		return Qnil;
  }

	return unmarshall_value(pos->value());
	__NIL_END
}
