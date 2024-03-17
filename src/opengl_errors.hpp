#pragma  once

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
#else
    #define PERIA_ASSERT(expr, msg) expr
    #define PERIA_ASSERT_GL(expr, expr_str, file, line)
#endif

#define GL_CALL(expr) gl_clear_errors(); \
    expr; \
    PERIA_ASSERT_GL(!gl_check_errors(), #expr, __FILE__, __LINE__)
