class Libvorbis < Formula
    desc "Vorbis General Audio Compression Codec"
    homepage "https://xiph.org/vorbis/"
    url "https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.6.tar.xz"
    sha256 "af00bb5a784e7c9e69f56823de4637c350643deedaf333d0fa86ecdba6fcb415"
  
    bottle do
      cellar :any
      rebuild 1
      sha256 "78dd589f9c2110c204e8762d3ee2899b209cded2bd372603be0fc8c748f52cd4" => :catalina
      sha256 "2fdfb811a94b56091aa3175708e92f7371c59a961a798fe8dfb2a82d0241406e" => :mojave
      sha256 "434729ebb5c161e3240dde60f752af6bbec324af3b7df87f802d48134db92afb" => :high_sierra
    end
  
    head do
      url "https://gitlab.xiph.org/xiph/vorbis.git"
  
      depends_on "autoconf" => :build
      depends_on "automake" => :build
      depends_on "libtool" => :build
    end
  
    depends_on "pkg-config" => :build
    depends_on "libogg"
  
    resource("oggfile") do
      url "https://upload.wikimedia.org/wikipedia/commons/c/c8/Example.ogg"
      sha256 "379071af4fa77bc7dacf892ad81d3f92040a628367d34a451a2cdcc997ef27b0"
    end
  
    def install
      system "./autogen.sh" if build.head?
      ENV["MACOSX_DEPLOYMENT_TARGET"] = "10.12"
      system "./configure", "--disable-dependency-tracking",
                            "--prefix=#{prefix}"
      system "make", "install"
    end
  
    test do
      (testpath/"test.c").write <<~EOS
        #include <stdio.h>
        #include <assert.h>
        #include "vorbis/vorbisfile.h"
        int main (void) {
          OggVorbis_File vf;
          assert (ov_open_callbacks (stdin, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) >= 0);
          vorbis_info *vi = ov_info (&vf, -1);
          printf("Bitstream is %d channel, %ldHz\\n", vi->channels, vi->rate);
          printf("Encoded by: %s\\n", ov_comment(&vf,-1)->vendor);
          return 0;
        }
      EOS
      testpath.install resource("oggfile")
      system ENV.cc, "test.c", "-I#{include}", "-L#{lib}", "-lvorbisfile",
                     "-o", "test"
      assert_match "2 channel, 44100Hz\nEncoded by: Xiph.Org libVorbis",
                   shell_output("./test < Example.ogg")
    end
  end