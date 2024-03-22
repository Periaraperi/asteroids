#pragma  once

/* OpenGL calls will be wrapped in a macro to check for errors
 * during debug builds.
 * For release builds we need to track down errors with RenderDoc
 * or other external tools
 * */

void gl_clear_errors();
bool gl_check_errors();

void log_error(const char* expr, const char* file, int line, const char* msg="");

#ifdef PERIA_DEBUG
    #define PERIA_ASSERT(expr, msg) \
        if (!(expr)) { \
            log_error(#expr, __FILE__, __LINE__, msg); \
            std::exit(EXIT_FAILURE); \
        }
    #define PERIA_ASSERT_GL(expr, expr_str, file, line) \
        if (!(expr)) { \
            log_error(expr_str, file, line); \
            std::exit(EXIT_FAILURE); \
        }

    #define GL_CALL(expr) gl_clear_errors(); \
        expr; \
        PERIA_ASSERT_GL(!gl_check_errors(), #expr, __FILE__, __LINE__)
#else
    #define PERIA_ASSERT(expr, msg) expr
    #define PERIA_ASSERT_GL(expr, expr_str, file, line)
    #define GL_CALL(expr) expr;
#endif

