class Sdl2Net < Formula
  desc "Small sample cross-platform networking library"
  homepage "https://www.libsdl.org/projects/SDL_net/"
  url "https://www.libsdl.org/projects/SDL_net/release/SDL2_net-2.0.1.tar.gz"
  sha256 "15ce8a7e5a23dafe8177c8df6e6c79b6749a03fff1e8196742d3571657609d21"

  bottle do
    sha256 cellar: :any, catalina: "920e892ba80cba3a99d4a15473351be5dc23f0d9445c28480c5dae904e8a8271"
    sha256 cellar: :any, mojave: "0631754a7016b3e6e175644cc7976cc22843f7b872e8f50662d0cb50a4264901"
    sha256 cellar: :any, high_sierra: "f193c7c2ae1b7f2c82cbbc9b83a16fc72d845c6396ecd33644eea19695a850ee"
    sha256 cellar: :any, sierra: "dc2b96762f77dd4d42fea1da4d4c2373692dd0a531f686f00de0dd4a6eed8df9"
    sha256 cellar: :any, el_capitan: "46d189ebe1f240381a9e8d99a2cb249e577cec98e6399e741e47275735a3471c"
    sha256 cellar: :any, yosemite: "2e2bcc1e1aac84b37ebb44398e463d9004764aa369489926cd07bb97cb9f60c4"
    sha256 cellar: :any, mavericks: "ebabcb8f4df6fdee7855a6e19080aea42d9909205b287312015179bb9b3f472a"
  end

  depends_on "pkg-config" => :build
  depends_on "sdl2"

  def install
    inreplace "SDL2_net.pc.in", "@prefix@", HOMEBREW_PREFIX
    ENV["MACOSX_DEPLOYMENT_TARGET"] = "10.12"
    system "./configure", "--disable-dependency-tracking",
                          "--prefix=#{prefix}", "--disable-sdltest"
    system "make", "install"
  end

  test do
    (testpath/"test.c").write <<~EOS
      #include <SDL2/SDL_net.h>

      int main()
      {
          int success = SDLNet_Init();
          SDLNet_Quit();
          return success;
      }
    EOS

    system ENV.cc, "-L#{lib}", "-lsdl2_net", "test.c", "-o", "test"
    system "./test"
  end
end