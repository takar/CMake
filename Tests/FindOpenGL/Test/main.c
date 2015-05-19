#ifdef __APPLE__
# pragma GCC diagnostic ignored "-Wdeprecated"
# include <OpenGL/gl.h>
# ifdef HAVE_GLU
#  include <OpenGL/glu.h>
# endif
#else
# ifdef _WIN32
#  include <windows.h>
# endif
# include <GL/gl.h>
# ifdef HAVE_GLU
#  include <GL/glu.h>
# endif
#endif

#ifdef __GNUC__
# pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

int main(int argc, char* argv[])
{
  (void)argv;
#ifdef HAVE_GLU
  /* Link to a GLU symbol that can be called without a context.  */
  gluErrorString(GLU_INVALID_ENUM);
#endif
  /* Link to a GL symbol without calling it since we have no context.  */
  return ((int)(&glGetError)) * (argc-1);
}
