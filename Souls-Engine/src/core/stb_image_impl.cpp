// stb_image 实现文件
// 注意：这个文件需要完整的 stb_image.h 才能工作
// 请从 https://github.com/nothings/stb 下载完整的 stb_image.h
// 并替换 extern/stb/stb_image.h

// 如果已经有完整的stb_image.h，取消下面的注释：
// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// 临时占位实现（仅用于编译通过，实际功能需要完整的stb_image.h）
#ifdef STB_IMAGE_IMPLEMENTATION
// 如果定义了STB_IMAGE_IMPLEMENTATION，说明已经包含了完整的stb_image.h
// 这里不需要额外代码
#else
// 占位实现（仅用于编译，不会实际工作）
extern "C" {
    typedef unsigned char stbi_uc;
    
    static const char* g_stbi_failure_reason = "stb_image.h not fully implemented. Please download from https://github.com/nothings/stb";
    
    stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
        (void)filename; (void)x; (void)y; (void)channels_in_file; (void)desired_channels;
        return nullptr;
    }
    
    void stbi_image_free(void *retval_from_stbi_load) {
        (void)retval_from_stbi_load;
    }
    
    const char *stbi_failure_reason(void) {
        return g_stbi_failure_reason;
    }
    
    void stbi_set_flip_vertically_on_load(int flag_true_if_should_flip) {
        (void)flag_true_if_should_flip;
    }
}
#endif

