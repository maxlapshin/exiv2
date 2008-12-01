static VALUE exiv2_metadata_delete(VALUE rb_key) {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	VALUE strkey = rb_funcall(rb_key, rb_intern("to_s"), 0);
	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();
	
	if(data.empty()) {
		return Qnil;
	}
	
    Exiv2::<%= kind %>Key key(STR(strkey));
    Exiv2::<%= kind %>Data::iterator pos = data.findKey(key);
    if (pos == data.end()) {
		return Qnil;
    }
	
	std::string v = pos->toString();
	data.erase(pos);
	return rb_str_new(v.c_str(), v.length());
	__NIL_END
}
