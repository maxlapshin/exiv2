require 'test/unit'
require File.dirname(__FILE__) + '/../lib/exiv2'
require 'tempfile'


class ImageTest < Test::Unit::TestCase
  
  def open_test_file(filename)
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
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f), "Image should be opened from IO::File"
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal nil, @img.exif["zeze"]
      assert_equal "3024", @img.exif["Exif.Photo.PixelXDimension"]
    end
    
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f.path), "Image should be opened from filename"
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal nil, @img.exif["zeze"]
      assert_equal "3024", @img.exif["Exif.Photo.PixelXDimension"]
    end
  end
  
  def test_write
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      assert_equal "FinePixS2Pro", @img.exif["Exif.Image.Model"]
      assert_equal "*istDs", @img.exif["Exif.Image.Model"] = "*istDs"
      assert @img.save

      assert @img = Exiv2::Image.new(f)
      assert_equal "*istDs", @img.exif["Exif.Image.Model"]
    end
  end

  def test_comment
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      assert_equal "My funny comment", @img.comment = "My funny comment"
      assert @img.save

      assert @img = Exiv2::Image.new(f)
      assert_equal "My funny comment", @img.comment
    end
  end
  
  def test_typehinting
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      assert @exif = @img.exif
      assert_equal 1, @exif["Exif.Image.Orientation"]
      assert_equal "Digital Camera FinePixS2Pro Ver1.00", @exif["Exif.Image.Software"]
      assert_equal 72, @exif["Exif.Image.XResolution"]
      assert_equal String, @exif["Exif.Image.XResolution"].class
      puts @exif["Exif.Image.XResolution"].class

      require 'rational'

      assert_equal 72, @exif["Exif.Image.XResolution"]
      assert_equal Numeric, @exif["Exif.Image.XResolution"].class
      puts @exif["Exif.Image.XResolution"].class
    end
  end

  def test_each
    open_test_file "exiv2-fujifilm-finepix-s2pro.jpg" do |f|
      assert @img = Exiv2::Image.new(f)
      i = 0
      @img.exif.each do |key, value|
        i = i + 1
        #puts "#{key} => #{value.inspect}"
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