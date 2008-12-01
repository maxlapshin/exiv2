module Exiv2
  class Tag
    inline do |builder|
      Exiv2.prepare_builder(builder)
      builder.prefix <<-EOF
      VALUE cTag;
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
      EOF
      
      builder.add_to_init "cTag = c;"
      
      builder.c_singleton <<-EOF
      static VALUE each() {
      	__BEGIN
      	iterate_tag_collection(Exiv2::ExifTags::ifdTagList());
      	iterate_tag_collection(Exiv2::ExifTags::exifTagList());
      	iterate_tag_collection(Exiv2::ExifTags::iopTagList());
      	iterate_tag_collection(Exiv2::ExifTags::gpsTagList());
      	return self;
      	__END
      }
      EOF
      
      builder.c_singleton <<-EOF
      static VALUE count() {
      	__BEGIN
      	return INT2NUM(
      	iterate_tag_collection(Exiv2::ExifTags::ifdTagList(), false) +
      	iterate_tag_collection(Exiv2::ExifTags::exifTagList(), false) +
      	iterate_tag_collection(Exiv2::ExifTags::iopTagList(), false) +
      	iterate_tag_collection(Exiv2::ExifTags::gpsTagList(), false)
      	);
      	__END
      }
      EOF
      
      builder.c <<-EOF
      static VALUE name() {
      	__BEGIN
      	Exiv2::TagInfo* tag;
      	Data_Get_Struct(self, Exiv2::TagInfo, tag);

      	return tag->name_ ? rb_str_new2(tag->name_) : Qnil;
      	__END
      }
      EOF
      
      builder.c <<-EOF
      static VALUE title() {
      	__BEGIN
      	Exiv2::TagInfo* tag;
      	Data_Get_Struct(self, Exiv2::TagInfo, tag);

      	return tag->title_ ? rb_str_new2(tag->title_) : Qnil;
      	__END
      }
      EOF
      
      builder.c <<-EOF
      static VALUE desc() {
      	__BEGIN
      	Exiv2::TagInfo* tag;
      	Data_Get_Struct(self, Exiv2::TagInfo, tag);

      	return tag->desc_ ? rb_str_new2(tag->desc_) : Qnil;
      	__END
      }
      EOF
      
      builder.c <<-EOF
      static VALUE section() {
      	__BEGIN
      	Exiv2::TagInfo* tag;
      	Data_Get_Struct(self, Exiv2::TagInfo, tag);

      	return rb_str_new2(Exiv2::ExifTags::sectionName(tag->sectionId_));
      	__END
      }
      EOF
      
      builder.c <<-EOF
      static VALUE ifd() {
      	__BEGIN
      	Exiv2::TagInfo* tag;
      	Data_Get_Struct(self, Exiv2::TagInfo, tag);

      	return rb_str_new2(Exiv2::ExifTags::ifdName(tag->ifdId_));
      	__END
      }
      EOF
    end
  end
end