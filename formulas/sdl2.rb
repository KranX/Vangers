class Sdl2 < Formula
    desc "Low-level access to audio, keyboard, mouse, joystick, and graphics"
    homepage "https://www.libsdl.org/"
    url "https://libsdl.org/release/SDL2-2.0.14.tar.gz"
    sha256 "d8215b571a581be1332d2106f8036fcb03d12a70bae01e20f424976d275432bc"
    license "Zlib"
    revision 1
  
    bottle do
      cellar :any
      sha256 cellar: :any, big_sur:       "ccde7145d4334d9274f9588e6b841bf3749729682e1d25f590bdcf7994dfdd89"
      sha256 cellar: :any, catalina:      "d6ae3300160c5bb495b78a5c5c0fc995f9e797e9cdd4b04ef77d59d45d2d694d"
      sha256 cellar: :any, mojave:        "4f3988fb3af0f370bc1648d6eb1d6573fd37381df0f3b9ee0874a49d6a7dec2e"
    end
  
    head do
      url "https://hg.libsdl.org/SDL", :using => :hg
  
      depends_on "autoconf" => :build
      depends_on "automake" => :build
      depends_on "libtool" => :build
    end
  
    on_linux do
      depends_on "pkg-config" => :build
    end
  
    def install
      # we have to do this because most build scripts assume that all SDL modules
      # are installed to the same prefix. Consequently SDL stuff cannot be
      # keg-only but I doubt that will be needed.
      inreplace %w[sdl2.pc.in sdl2-config.in], "@prefix@", HOMEBREW_PREFIX
  
      system "./autogen.sh" if build.head?
  
      args = %W[--prefix=#{prefix} --without-x --enable-hidapi]
      ENV["MACOSX_DEPLOYMENT_TARGET"] = "10.12"
      system "./configure", *args
      system "make", "install"
    end
  
    test do
      system bin/"sdl2-config", "--version"
    end
  end