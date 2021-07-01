#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES2/gl2.h>
#include <malloc.h>

GLfloat triangle_vertices[] = {
//    x,     y,    u,                           v,
   1.0f,  1.0f,   (137.0f*2.0f+1.0f)/1024.0f,  ( 70.0f*2.0f+1.0f)/1024.0f,
   1.0f, -1.0f,   ( 57.0f*2.0f+1.0f)/1024.0f,  (170.0f*2.0f+1.0f)/1024.0f,
  -1.0f, -1.0f,   (226.0f*2.0f+1.0f)/1024.0f,  (171.0f*2.0f+1.0f)/1024.0f,
  -1.0f,  1.0f,   (296.0f*2.0f+1.0f)/1024.0f,  (305.0f*2.0f+1.0f)/1024.0f,
  -1.0f, -1.0f,   (254.0f*2.0f+1.0f)/1024.0f,  (413.0f*2.0f+1.0f)/1024.0f,
   1.0f,  1.0f,   (368.0f*2.0f+1.0f)/1024.0f,  (411.0f*2.0f+1.0f)/1024.0f,
};

const GLchar *vertex_shader_source = 
	 "attribute vec2 aVertex;"
	 "attribute vec2 aTexel;"

	 "varying vec2 vTexel;"

   "void main() {"
	 "   vTexel = aTexel;"
	 "   gl_Position = vec4(aVertex, 0.0, 1.0);"
	 "}";
		
const GLchar *fragment_shader_source =
	 "precision mediump float;"

	 "varying vec2 vTexel;"

	 "uniform sampler2D uTexmap;"

	 "void main() {"
	 "   gl_FragColor = texture2D(uTexmap, vec2(vTexel.s, vTexel.t));"
	 "}";

   GLuint vbo;
   GLint  handle_aVertex;
   GLint  handle_aTexel;
   GLint  handle_uTexmap;
   GLuint texture_handle[1];

//-------------------------------------------------------------------------------
void OnDrawFrame() {

   static int page_flip = 0;
   if (page_flip++ & 1) {
      glClearColor(0.9f, 0.9f, 0.0f, 1.0f);
   } else {
      glClearColor(0.9f, 0.0f, 0.0f, 1.0f);
   }

   glClear(GL_COLOR_BUFFER_BIT);

   glDrawArrays(GL_TRIANGLES, 0, 6);
}
//-------------------------------------------------------------------------------
void LoadTexture() {
   int w, h;
   char *img_data;

   img_data = emscripten_get_preloaded_image_data("assets/texture_sheet00.png", &w, &h);
   glGenTextures(1, texture_handle);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, texture_handle[0]);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
   free(img_data);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
//-------------------------------------------------------------------------------
int main() {

   EmscriptenWebGLContextAttributes attr;
   emscripten_webgl_init_context_attributes(&attr);

   // a context is required to use the WebGL API
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#my_canvas", &attr);

   // binds the WebGL API to the context
   emscripten_webgl_make_context_current(ctx);
   emscripten_set_canvas_element_size("#my_canvas", 640, 480);

   glViewport(0, 0, 640, 480);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glGenBuffers(1, &vbo);

   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

   GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
   glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
   glCompileShader(vertex_shader);

   GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
   glCompileShader(fragment_shader);

   GLuint shader_program = glCreateProgram();
   glAttachShader(shader_program, vertex_shader);
   glAttachShader(shader_program, fragment_shader);
   glLinkProgram(shader_program);
   glUseProgram(shader_program); 

   handle_aVertex = glGetAttribLocation(shader_program, "aVertex");
   handle_aTexel = glGetAttribLocation(shader_program, "aTexel");
   handle_uTexmap = glGetUniformLocation(shader_program, "uTexmap");

   LoadTexture();

   glVertexAttribPointer(handle_aVertex, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, 0);
   glEnableVertexAttribArray(handle_aVertex);   

   glVertexAttribPointer(handle_aTexel, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void *)(2*sizeof(float)));
   glEnableVertexAttribArray(handle_aTexel);   

   glUniform1i(handle_uTexmap, 0);

   emscripten_set_main_loop(OnDrawFrame, 0, 1);

   return 0;
}
//-------------------------------------------------------------------------------

