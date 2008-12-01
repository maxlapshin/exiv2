static VALUE exiv2_metadata_is_empty() {
	__BEGIN
	rbImage* image;
	Data_Get_Struct(self, rbImage, image);

	Exiv2::<%= kind %>Data &data = image->image-><%= kind.downcase %>Data();
	
	return data.empty() ? Qtrue : Qfalse;
	__NIL_END
}
