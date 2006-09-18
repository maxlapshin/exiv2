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
      assert @img = Exiv2::Image.new(f)
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal nil, @img.exif["zeze"]
      assert_equal "3024", @img.exif["Exif.Photo.PixelXDimension"]
    end
  end
  
  def test_write
    test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      assert_equal "*istDs", @img.exif["Exif.Image.Model"] = "*istDs"
      assert @img.save

      assert @img = Exiv2::Image.new(f)
      assert_equal "*istDs", @img.exif["Exif.Image.Model"]
    end
  end

  def test_comment
    test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      assert_equal "My funny comment", @img.comment = "My funny comment"
      assert @img.save

      assert @img = Exiv2::Image.new(f)
      assert_equal "My funny comment", @img.comment
    end
  end

  def test_each
    test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      i = 0
      @img.exif.each do |key, value|
        i = i + 1
        assert key
        assert value
      end
      assert_equal 74, i

      i = 0
      @img.exif.each "Exif.Fujifilm" do |key, value|
        i = i + 1
        assert key
        assert value
      end
      assert_equal 16, i
    end
  end
end