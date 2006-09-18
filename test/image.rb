require 'test/unit'
require File.dirname(__FILE__) + '/../lib/exiv2'
require 'tempfile'

class ImageTest < Test::Unit::TestCase
  
  def test_file(filename)
    test_file_name = File.dirname(__FILE__) + "/data/file_test"
    open(test_file_name, "w+") do |f|
      open(File.dirname(__FILE__) + "/data/#{filename}") do |real|
        f << real.read
      end
    end
    begin
      yield test_file_name
    rescue
    end
    File.unlink(test_file_name)
  end
  
  def test_open
    test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @exif = Exiv2::Image.new(f)
      assert_equal "FinePixS2Pro", @exif["Exif.Image.Model"]
      assert_equal nil, @exif["zeze"]
      assert_equal "3024", @exif["Exif.Photo.PixelXDimension"]
    end
  end
  
  def test_write
    test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @exif = Exiv2::Image.new(f)
      assert_equal "*istDs", @exif["Exif.Image.Model"] = "*istDs"
      assert @exif.save

      assert @exif = Exiv2::Image.new(f)
      assert_equal "*istDs", @exif["Exif.Image.Model"]
    end
  end
end